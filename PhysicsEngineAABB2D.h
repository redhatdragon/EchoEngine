#ifndef PHYSICS_GUARD
#define PHYSICS_GUARD

#include "Vec.h"
#include "FlatBuffer.h"
#include <time.h>

struct BodyAABB {
	bool solid;
	Vec2D<int32_t> pos, siz, vel;

	__forceinline void simulate() {
		pos += vel;
	}
	__forceinline void reverseSimulate() {
		pos -= vel;
	}
	__forceinline bool collidesWith(BodyAABB& other) {
		if (pos.x + siz.x < other.pos.x || pos.x > other.pos.x + other.siz.x ||
			pos.y + siz.y < other.pos.y || pos.y > other.pos.y + other.siz.y)
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

template<uint32_t width, uint32_t height, uint32_t hash_width, uint32_t max_bodies_per_hash = 64>
class SpatialHashTable {
	FlatBuffer<BodyID, max_bodies_per_hash> hash[width * height];
	void addBodyToHash(BodyID id, uint32_t index) {
		hash[index].push(id);
	}
	void removeBodyFromHash(BodyID id, uint32_t index) {
		uint32_t len = hash[index].count;
		for (uint32_t i = 0; i < len; i++) {
			if (hash[index][i].id == id.id) {
				hash[index][i] = hash[index][len - 1];
				hash[index].pop();
				return;
			}
		}
		printf("SpatialHashTable::removeBodyFromHash - Error: body not found in hash!\n");
	}
public:
	void addBody(BodyID id, const Vec2D<int32_t>& pos, const Vec2D<int32_t>& siz) {
		uint32_t startRight = pos.x / hash_width, startDown = pos.y / hash_width;
		uint32_t endRight = (pos.x + siz.x) / hash_width, endDown = (pos.y + siz.y) / hash_width;
		for (uint32_t y = startDown; y <= endDown; y++)
			for (uint32_t x = startRight; x <= endRight; x++)
				addBodyToHash(id, x + y * width);
	}
	void removeBody(BodyID id, const Vec2D<int32_t>& pos, const Vec2D<int32_t>& siz) {
		uint32_t startRight = pos.x / hash_width, startDown = pos.y / hash_width;
		uint32_t endRight = (pos.x + siz.x) / hash_width, endDown = (pos.y + siz.y) / hash_width;
		for (uint32_t y = startDown; y <= endDown; y++)
			for (uint32_t x = startRight; x <= endRight; x++)
				removeBodyFromHash(id, x + y * width);
	}
	bool hashHasID(uint32_t index, BodyID id) {
		uint32_t len = hash[index].count;
		for (uint32_t i = 0; i < len; i++)
			if (hash[index][i].id == id.id)
				return true;
		return false;
	}
	__forceinline std::vector< FlatBuffer<BodyID, max_bodies_per_hash>*> getHashes(Vec2D<int32_t>& pos, Vec2D<int32_t>& siz) {
		std::vector< FlatBuffer<BodyID, max_bodies_per_hash>*> returnValue;
		uint32_t startRight = pos.x / hash_width, startDown = pos.y / hash_width;
		uint32_t endRight = (pos.x + siz.x) / hash_width, endDown = (pos.y + siz.y) / hash_width;
		for (uint32_t y = startDown; y <= endDown; y++)
			for (uint32_t x = startRight; x <= endRight; x++)
				returnValue.push_back(&hash[x + y * width]);
		return returnValue;
	}
	// This was complicated and may need another look over
	void getIDs(const Vec2D<int32_t>& pos, const Vec2D<int32_t>& siz, std::vector<BodyID>& returnValue) {
		uint32_t startRight = pos.x / hash_width, startDown = pos.y / hash_width;
		uint32_t endRight = (pos.x + siz.x) / hash_width, endDown = (pos.y + siz.y) / hash_width;
		for (uint32_t y = startDown; y <= endDown; y++) {
			for (uint32_t x = startRight; x <= endRight; x++) {
				auto len = hash[x + y * width].count;
				for (uint32_t i = 0; i < len; i++) {
					auto retValueLen = returnValue.size();
					bool hasID = false;
					for (size_t j = 0; j < retValueLen; j++) {
						if (returnValue[j].id == hash[x + y * width][i].id) {
							hasID = true;
							break;
						}
					}
					if (hasID == false) {
						returnValue.push_back(hash[x + y * width][i]);
					}

				}
			}
		}
	}
};

template<uint32_t width, uint32_t height, uint32_t hash_width, uint32_t max_bodies_per_hash = 64>
class PhysicsEngineAABB2D {
	static constexpr uint32_t max_bodies = 100000;
	static constexpr uint32_t unit_size = 256;  //used as a pretend "1" normalized value to emulate decimal
	FlatFlaggedBuffer<BodyAABB, max_bodies> bodies = FlatFlaggedBuffer<BodyAABB, max_bodies>();
	FlatBuffer<void*, max_bodies> userData;
	FlatBuffer<FlatBuffer<BodyID, 100>, max_bodies> overlappingBodyIDs;
	//SpatialHashTable<300, 300, 128*unit_size> spatialHashTable;
	SpatialHashTable<width, height, hash_width* unit_size, max_bodies_per_hash> spatialHashTable;
	float timeFromStepping = 0;
public:
	void init() {

	}
	BodyID addBodyRect(int32_t x, int32_t y, int32_t w, int32_t h) {
		x *= unit_size; y *= unit_size;
		w *= unit_size; h *= unit_size;
		BodyID retValue;
		BodyAABB body = { true, { x,y }, { w,h }, { 0,0 } };
		retValue.id = bodies.insert(body);
		spatialHashTable.addBody(retValue, body.pos, body.siz);
		overlappingBodyIDs[retValue.id].clear();
		return retValue;
	}
	void removeBody(BodyID bodyID) {
		BodyAABB& body = bodies[bodyID.id];
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

	inline std::vector<BodyID> getBodiesInRectRough(Vec2D<int32_t>& pos, Vec2D<int32_t>& siz) {
		pos *= unit_size; siz *= unit_size;
		std::vector<BodyID> retValue;



		auto hashes = spatialHashTable.getHashes(pos, siz);
		for (auto* hash : hashes) {
			uint32_t hashSize = hash->count;
			for (uint32_t i = 0; i < hashSize; i++) {
				BodyID bodyID = (*hash)[i];
				for (BodyID e : retValue) {
					if (e == (*hash)[i]) goto skip;
				}
				retValue.push_back(bodyID);
			skip:
				continue;
			}
		}

		return retValue;
	}

	void addVelocity(BodyID id, float vx, float vy) {
		BodyAABB* body = &bodies[id.id];
		body->vel.x += vx * unit_size;
		body->vel.y += vy * unit_size;
	}
	void setVelocity(BodyID id, float vx, float vy) {
		BodyAABB* body = &bodies[id.id];
		body->vel.x = vx * unit_size;
		body->vel.y = vy * unit_size;
	}
	void setVelocity(BodyID id, FixedPoint<unit_size> vx, FixedPoint<unit_size> vy) {
		BodyAABB* body = &bodies[id.id];
		body->vel.x = vx.getRaw();
		body->vel.y = vy.getRaw();
	}

	bool getSolid(BodyID id) {
		return bodies[id.id].solid;
	}
	void setSolid(BodyID id, bool isTrue) {
		bodies[id.id].solid = isTrue;
	}

	template<typename T>
	Vec2D<T> getPos(BodyID id) {
		Vec2D<int32_t> bodyPos = bodies[id.id].pos;
		Vec2D<T> pos = { bodyPos.x / unit_size, bodyPos.y / unit_size };
		return pos;
	}
	template<typename T>
	Vec2D<T> getSize(BodyID id) {
		Vec2D<int32_t> bodySiz = bodies[id.id].siz;
		Vec2D<T> siz = { bodySiz.x / unit_size, bodySiz.y / unit_size };
		return siz;
	}

	float getTime() {
		return timeFromStepping;
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
			if (overlappingBodyIDs[i].count && bodies[i].vel.isZero() == false && bodies[i].solid
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

	Vec2D<int32_t> getMaxPositions() {
		constexpr int32_t ceilx = width * hash_width * unit_size;
		constexpr int32_t ceily = height * hash_width * unit_size;
		return { ceilx, ceily };
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
			if (bodies[otherBody.id].solid)
				return true;
		}
		return false;
	}
};

#endif