#ifndef PHYSICS_GUARD
#define PHYSICS_GUARD

#include "Vec.h"
#include "FlatBuffer.h"
#include <time.h>

//TODO: Consider expanding for future utility.

struct BodyAABB {
	Vec3D<int32_t> pos, siz, vel;
	uint8_t isSolid;

	__forceinline void simulate() {
		pos += vel;
	}
	__forceinline void reverseSimulate() {
		pos -= vel;
	}
	__forceinline bool collidesWith(BodyAABB& other) {
		if (pos.x + siz.x < other.pos.x || pos.x > other.pos.x + other.siz.x ||
			pos.y + siz.y < other.pos.y || pos.y > other.pos.y + other.siz.y ||
			pos.z + siz.z < other.pos.z || pos.z > other.pos.z + other.siz.z)
			return false;
		return true;
	}
};

struct BodyID {
	uint32_t id;
	bool operator==(BodyID other) {
		if (id == other.id)
			return true;
		return false;
	}
};

template<uint32_t width, uint32_t height, uint32_t depth, uint32_t hash_width, uint32_t max_bodies_per_hash = 64>
class SpatialHashTable {
	//FlatBuffer<BodyID, max_bodies_per_hash> hash[width * height * depth];
	FlatBuffer<BodyID, max_bodies_per_hash> hash[width][height][depth];  //TODO: perf test this compared to manual flat variant..
	//void addBodyToHash(BodyID id, uint32_t index) {
	//	hash[index].push(id);
	//}
	inline void addBodyToHash(BodyID id, uint32_t x, uint32_t y, uint32_t z) {
		hash[x][y][z].push(id);
	}
	void removeBodyFromHash(BodyID id, uint32_t x, uint32_t y, uint32_t z) {
		uint32_t len = hash[x][y][z].count;
		for (uint32_t i = 0; i < len; i++) {
			if (hash[x][y][z][i].id == id.id) {
				hash[x][y][z][i] = hash[x][y][z][len - 1];
				hash[x][y][z].pop();
				return;
			}
		}
		printf("SpatialHashTable::removeBodyFromHash - Error: body not found in hash!\n");
	}
public:
	void addBody(BodyID id, const Vec3D<int32_t>& pos, const Vec3D<int32_t>& siz) {
		uint32_t startRight = pos.x / hash_width, startDown = pos.y / hash_width;
		uint32_t endRight = (pos.x + siz.x) / hash_width, endDown = (pos.y + siz.y) / hash_width;
		uint32_t startForward = (pos.z / hash_width), endForward = (pos.z + siz.z) / hash_width;
		for(uint32_t z = startForward; z <= endForward; z++)
			for (uint32_t y = startDown; y <= endDown; y++)
				for (uint32_t x = startRight; x <= endRight; x++)
					addBodyToHash(id, x, y, z);
	}
	void removeBody(BodyID id, const Vec3D<int32_t>& pos, const Vec3D<int32_t>& siz) {
		uint32_t startRight = pos.x / hash_width, startDown = pos.y / hash_width;
		uint32_t endRight = (pos.x + siz.x) / hash_width, endDown = (pos.y + siz.y) / hash_width;
		uint32_t startForward = (pos.z / hash_width), endForward = (pos.z + siz.z) / hash_width;
		for (uint32_t z = startForward; z <= endForward; z++)
			for (uint32_t y = startDown; y <= endDown; y++)
				for (uint32_t x = startRight; x <= endRight; x++)
					removeBodyFromHash(id, x, y, z);
	}
	//TODO: consider making this return through param as to potentially allow .reserve() before calling.
	__forceinline std::vector< FlatBuffer<BodyID, max_bodies_per_hash>*> getHashes(const Vec2D<int32_t>& pos, const Vec2D<int32_t>& siz) {
		std::vector< FlatBuffer<BodyID, max_bodies_per_hash>*> returnValue;
		uint32_t startRight = pos.x / hash_width, startDown = pos.y / hash_width;
		uint32_t endRight = (pos.x + siz.x) / hash_width, endDown = (pos.y + siz.y) / hash_width;
		uint32_t startForward = (pos.z / hash_width), endForward = (pos.z + siz.z) / hash_width;
		for (uint32_t z = startForward; z <= endForward; z++)
			for (uint32_t y = startDown; y <= endDown; y++)
				for (uint32_t x = startRight; x <= endRight; x++)
					returnValue.push_back(&hash[x][y][z]);
		return returnValue;
	}
	// This was complicated and may need another look over
	void getIDs(const Vec3D<int32_t>& pos, const Vec3D<int32_t>& siz, std::vector<BodyID>& returnValue) {
		uint32_t startRight = pos.x / hash_width, startDown = pos.y / hash_width;
		uint32_t endRight = (pos.x + siz.x) / hash_width, endDown = (pos.y + siz.y) / hash_width;
		uint32_t startForward = (pos.z / hash_width), endForward = (pos.z + siz.z) / hash_width;
		for (uint32_t z = startForward; z <= endForward; z++)
			for (uint32_t y = startDown; y <= endDown; y++)
				for (uint32_t x = startRight; x <= endRight; x++) {
					auto len = hash[x][y][z].count;
					for (uint32_t i = 0; i < len; i++) {
						auto retValueLen = returnValue.size();
						bool hasID = false;
						for (size_t j = 0; j < retValueLen; j++) {
							if (returnValue[j].id == hash[x][y][z][i].id) {
								hasID = true;
								break;
							}
						}
						if (hasID == false) {
							returnValue.push_back(hash[x][y][z][i]);
						}

					}
				}
	}
};

template<uint32_t width, uint32_t height, uint32_t depth, uint32_t hash_width, uint32_t max_bodies_per_hash = 64>
class PhysicsEngineAABB3D {
	static constexpr uint32_t max_bodies = 100000;
	static constexpr uint32_t unit_size = 256;  //used as a pretend "1" normalized value to emulate decimal
	FlatFlaggedBuffer<BodyAABB, max_bodies> bodies = FlatFlaggedBuffer<BodyAABB, max_bodies>();
	FlatBuffer<void*, max_bodies> userData;
	FlatBuffer<FlatBuffer<BodyID, 100>, max_bodies> overlappingBodyIDs;
	SpatialHashTable<width, height, depth, hash_width * unit_size, max_bodies_per_hash> spatialHashTable;
	float timeFromStepping = 0;
public:
	void init() {

	}
	BodyID addBodyBox(int32_t x, int32_t y, int32_t z, int32_t w, int32_t h, int32_t d) {
		x *= unit_size; y *= unit_size; z *= unit_size;
		w *= unit_size; h *= unit_size; d *= unit_size;
		BodyID retValue;
		BodyAABB body = { { x,y,z }, { w,h,d }, { 0,0,0 } };
		retValue.id = bodies.insert(body);
		spatialHashTable.addBody(retValue, body.pos, body.siz);
		overlappingBodyIDs[retValue.id].clear();
		return retValue;
	}
	void removeBody(BodyID bodyID) {
		const BodyAABB& body = bodies[bodyID.id];
		spatialHashTable.removeBody(bodyID, body.pos, body.siz);
		bodies.decrementCount();
		bodies.setInvalid(bodyID.id);
	}

	void* getUserData(BodyID id) {
		return userData[id.id];
	}
	void setUserData(BodyID id, void* data) {
		userData[id.id] = data;
	}

	FlatBuffer<BodyID, 100>& getOverlappingBodies(BodyID id) {
		return overlappingBodyIDs[id.id];
	}

	inline std::vector<BodyID> getBodiesInRectRough(Vec3D<int32_t>& pos, Vec3D<int32_t>& siz) {
		pos *= unit_size; siz *= unit_size;
		std::vector<BodyID> retValue;
		auto hashes = spatialHashTable.getHashes(pos, siz);
		for (auto* hash : hashes) {
			uint32_t hashSize = hash->count;
			for (uint32_t i = 0; i < hashSize; i++) {
				BodyID bodyID = (*hash)[i];
				for (BodyID e : retValue)
					if (e == (*hash)[i]) goto skip;
				retValue.push_back(bodyID);
			skip:
				continue;
			}
		}

		return retValue;
	}

	void addVelocity(BodyID id, float vx, float vy, float vz) {
		BodyAABB* body = &bodies[id.id];
		body->vel.x += vx * unit_size;
		body->vel.y += vy * unit_size;
		body->vel.z += vz * unit_size;
	}
	void setVelocity(BodyID id, float vx, float vy, float vz) {
		BodyAABB* body = &bodies[id.id];
		body->vel.x = vx * unit_size;
		body->vel.y = vy * unit_size;
		body->vel.z = vz * unit_size;
	}
	void setVelocity(BodyID id, FixedPoint<unit_size> vx, FixedPoint<unit_size> vy, FixedPoint<unit_size> vz) {
		BodyAABB* body = &bodies[id.id];
		body->vel.x = vx.getRaw();
		body->vel.y = vy.getRaw();
		body->vel.z = vz.getRaw();
	}

	bool getSolid(BodyID id) {
		return bodies[id.id].isSolid;
	}
	void setSolid(BodyID id, bool isTrue) {
		bodies[id.id].isSolid = isTrue;
	}

	template<typename T>
	Vec3D<T> getPos(BodyID id) {
		Vec3D<int32_t> bodyPos = bodies[id.id].pos;
		Vec3D<T> pos = { bodyPos.x / unit_size, bodyPos.y / unit_size, bodyPos.z / unit_size };
		return pos;
	}
	template<typename T>
	Vec3D<T> getSize(BodyID id) {
		Vec3D<int32_t> bodySiz = bodies[id.id].siz;
		Vec3D<T> siz = { bodySiz.x / unit_size, bodySiz.y / unit_size, bodySiz.z / unit_size };
		return siz;
	}

	float getTime() {
		return timeFromStepping;
	}

	static constexpr inline int64_t getSizeofBroad() {
		return sizeof(spatialHashTable);
	}
	static constexpr inline int64_t getSizeofBodies() {
		return sizeof(bodies);
	}

	uint32_t lastBodyIndex;
	inline void simulate() {
		uint32_t bodyCount = bodies.getCount();
		if (bodyCount == 0) return;
		uint32_t validBodyCount = 0;
		for (uint32_t i = 0; true; i++) {
			if (bodies.getIsValid(i) == false)
				continue;
			validBodyCount++;
			if (bodies[i].vel.isZero()) {
				if (validBodyCount >= bodyCount) {
					lastBodyIndex = i;
					break;
				}
				continue;
			}
			spatialHashTable.removeBody({ i }, bodies[i].pos, bodies[i].siz);
			bodies[i].simulate();
			spatialHashTable.addBody({ i }, bodies[i].pos, bodies[i].siz);
			if (validBodyCount >= bodyCount) {
				lastBodyIndex = i;
				break;
			}
		}
	}
	inline void detect() {
		uint32_t bodyCount = bodies.getCount();
		if (bodyCount == 0) return;
		for (uint32_t i = 0; i <= lastBodyIndex; i++) {
			if (bodies.getIsValid(i) == false)
				continue;
			overlappingBodyIDs[i].clear();
		}

		for (uint32_t i = 0; i <= lastBodyIndex; i++) {
			if (bodies.getIsValid(i) == false)
				continue;

			static std::vector<BodyID> IDs;
			IDs.resize(0);
			spatialHashTable.getIDs(bodies[i].pos, bodies[i].siz, IDs);
			size_t IDCount = IDs.size();
			for (size_t j = 0; j < IDCount; j++) {
				if (i == IDs[j].id)
					continue;
				if (bodies[i].collidesWith(bodies[IDs[j].id])) {
					overlappingBodyPushIfUnique(i, IDs[j]);
					//overlappingBodyPushIfUnique(IDs[j].id, { i });
				}
			}
		}
	}
	inline void resolve() {
		uint32_t bodyCount = bodies.getCount();
		if (bodyCount == 0) return;
		for (uint32_t i = 0; i <= lastBodyIndex; i++) {
			if (bodies.getIsValid(i) == false)
				continue;
			bool isSolid = getSolid({ i });
			if (overlappingBodyIDs[i].count && bodies[i].vel.isZero() == false && getSolid({ i })
				&& areAnyOverlappingBodiesSolid(i) == true) {
				spatialHashTable.removeBody({ i }, bodies[i].pos, bodies[i].siz);
				bodies[i].reverseSimulate();
				spatialHashTable.addBody({ i }, bodies[i].pos, bodies[i].siz);
			}
		}
	}

	void tick() {
		clock_t c = clock();
		simulate();
		detect();
		resolve();
		timeFromStepping = (float)(clock() - c);
	}

private:
	void overlappingBodyPushIfUnique(uint32_t index, BodyID id) {
		for (uint32_t i = 0; i < overlappingBodyIDs[index].count; i++)
			if (overlappingBodyIDs[index][i].id == id.id)
				return;
		overlappingBodyIDs[index].push(id);
	}
	bool areAnyOverlappingBodiesSolid(uint32_t bodyIndex) {
		uint32_t overlappingCount = overlappingBodyIDs[bodyIndex].count;
		for (uint32_t j = 0; j < overlappingCount; j++) {
			BodyID otherBody = overlappingBodyIDs[bodyIndex][j];
			//if (bodies[otherBody.id].solid)
			if(getSolid(otherBody) == true)
				return true;
		}
		return false;
	}
};

#endif