#pragma once

#include "FlatBuffer.h"
#include "FixedPoint.h"
#include <time.h>

template<typename T>
struct Vec2D {
	T x, y;
	__forceinline void operator+=(Vec2D& other) {
		x += other.x;
		y += other.y;
	}
	__forceinline void operator-=(Vec2D& other) {
		x -= other.x;
		y -= other.y;
	}
	__forceinline void operator/=(T num) {
		x /= num;
		y /= num;
	}
	__forceinline void operator*=(T num) {
		x = x * num;
		y = y * num;
	}
	__forceinline Vec2D<T> operator+(Vec2D& other) {
		return { x + other.x, y + other.y };
	}
	__forceinline Vec2D<T> operator-(Vec2D& other) {
		return { x - other.x, y - other.y };
	}
	__forceinline Vec2D<T> operator/(T num) {
		return { x / num, y / num };
	}
	__forceinline Vec2D<T> operator*(T num) {
		return { x * num, y * num };
	}
	__forceinline bool isZero() {
		if (x == 0 && y == 0)
			return true;
		return false;
	}
	__forceinline void normalize(uint32_t unit = 1) {
		auto dist = getDistance();
		if (dist != 0) {
			x /= dist / unit; y /= dist / unit;
		}
	}
	__forceinline FixedPoint<> getDistance() {
		return sqrt(x * x + y * y);
	}
	__forceinline double getDistanceFrom(Vec2D& other) {
		return (*this - other).getDistance();
	}
};

struct BodyAABB {
	bool solid;
	Vec2D<FixedPoint<>> pos, siz, vel;

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

template<uint32_t width, uint32_t height, uint32_t hashWidth, uint32_t max_bodies_per_hash = 64>
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
	void addBody(BodyID id, const Vec2D<FixedPoint<>>& pos, const Vec2D<FixedPoint<>>& siz) {
		uint32_t startRight = pos.x.getAsInt() / hashWidth, startDown = pos.y.getAsInt() / hashWidth;
		uint32_t endRight = (pos.x + siz.x).getAsInt() / hashWidth, endDown = (pos.y + siz.y).getAsInt() / hashWidth;
		for (uint32_t y = startDown; y <= endDown; y++)
			for (uint32_t x = startRight; x <= endRight; x++)
				addBodyToHash(id, x + y * width);
	}
	void removeBody(BodyID id, const Vec2D<FixedPoint<>>& pos, const Vec2D<FixedPoint<>>& siz) {
		uint32_t startRight = pos.x.getAsInt() / hashWidth, startDown = pos.y.getAsInt() / hashWidth;
		uint32_t endRight = (pos.x + siz.x).getAsInt() / hashWidth, endDown = (pos.y + siz.y).getAsInt() / hashWidth;
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
	__forceinline std::vector< FlatBuffer<BodyID, max_bodies_per_hash>*> getHashes(Vec2D<FixedPoint<>>& pos, Vec2D<FixedPoint<>>& siz) {
		std::vector< FlatBuffer<BodyID, max_bodies_per_hash>*> returnValue;
		uint32_t startRight = pos.x.getAsInt() / hashWidth, startDown = pos.y.getAsInt() / hashWidth;
		uint32_t endRight = (pos.x + siz.x).getAsInt() / hashWidth, endDown = (pos.y + siz.y).getAsInt() / hashWidth;
		for (uint32_t y = startDown; y <= endDown; y++)
			for (uint32_t x = startRight; x <= endRight; x++)
				returnValue.push_back(&hash[x + y * width]);
		return returnValue;
	}
	// This was complicated and may need another look over
	void getIDs(const Vec2D<FixedPoint<>>& pos, const Vec2D<FixedPoint<>>& siz, std::vector<BodyID>& returnValue) {
		FixedPoint<> startRight = pos.x / hashWidth, startDown = pos.y / hashWidth;
		FixedPoint<> endRight = (pos.x + siz.x) / hashWidth, endDown = (pos.y + siz.y) / hashWidth;
		for (uint32_t y = startDown.getAsInt(); y <= endDown.getAsInt(); y++) {
			for (uint32_t x = startRight.getAsInt(); x <= endRight.getAsInt(); x++) {
				uint32_t len = hash[x + y * width].count;
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

class PhysicsEngine {
	static constexpr uint32_t max_bodies = 100000;
	//static constexpr uint32_t unit_size = 256;  //used as a pretend "1" normalized value to emulate decimal
	FlatFlaggedBuffer<BodyAABB, max_bodies> bodies = FlatFlaggedBuffer<BodyAABB, max_bodies>();
	FlatBuffer<void*, max_bodies> userData;
	FlatBuffer<FlatBuffer<BodyID, 100>, max_bodies> overlappingBodyIDs;
	//SpatialHashTable<300, 300, 128*unit_size> spatialHashTable;
	SpatialHashTable<2000, 2000, 128 * FIXED_POINT_DEFAULT_SCALE> spatialHashTable;
	float timeFromStepping = 0;
public:
	void init() {

	}
	BodyID addBodyRect(FixedPoint<> x, FixedPoint<> y, FixedPoint<> w, FixedPoint<> h) {
		BodyID retValue;
		BodyAABB body = { true, { x,y }, { w,h }, { (uint32_t)0,(uint32_t)0 } };
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

	inline std::vector<BodyID> getBodiesInRectRough(Vec2D<FixedPoint<>>& pos, Vec2D<FixedPoint<>>& siz) {
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

	void addVelocity(BodyID id, FixedPoint<> vx, FixedPoint<> vy) {
		BodyAABB* body = &bodies[id.id];
		body->vel.x += vx;
		body->vel.y += vy;
	}
	void setVelocity(BodyID id, FixedPoint<> vx, FixedPoint<> vy) {
		BodyAABB* body = &bodies[id.id];
		body->vel.x = vx;
		body->vel.y = vy;
	}

	bool getSolid(BodyID id) {
		return bodies[id.id].solid;
	}
	void setSolid(BodyID id, bool isTrue) {
		bodies[id.id].solid = isTrue;
	}

	template<typename T>
	Vec2D<T> getPos(BodyID id) {
		Vec2D<FixedPoint<>> bodyPos = bodies[id.id].pos;
		Vec2D<T> pos = { bodyPos.x.getAsInt(), bodyPos.y.getAsInt() };
		return pos;
	}
	template<typename T>
	Vec2D<T> getSize(BodyID id) {
		Vec2D<FixedPoint<>> bodySiz = bodies[id.id].siz;
		Vec2D<T> siz = { bodySiz.x.getAsInt(), bodySiz.y.getAsInt() };
		return siz;
	}

	float getTime() {
		return timeFromStepping;
	}

	inline void simulate() {
		uint32_t bodyCount = bodies.getCount();
		if (bodyCount == 0) return;
		uint32_t validBodyCount = 0;
		for (uint32_t i = 0; true; i++) {
			if (bodies.getIsValid(i) == false)
				continue;
			validBodyCount++;
			if (bodies[i].vel.isZero()) {
				if (validBodyCount >= bodyCount) break;
				continue;
			}
			spatialHashTable.removeBody({ i }, bodies[i].pos, bodies[i].siz);
			bodies[i].simulate();
			spatialHashTable.addBody({ i }, bodies[i].pos, bodies[i].siz);
			if (validBodyCount >= bodyCount) break;
		}
	}
	inline void detect() {
		uint32_t bodyCount = bodies.getCount();
		if (bodyCount == 0) return;
		uint32_t validBodyCount = 0;
		for (uint32_t i = 0; true; i++) {
			if (bodies.getIsValid(i) == false)
				continue;
			validBodyCount++;
			overlappingBodyIDs[i].clear();
			if (validBodyCount >= bodyCount) break;
		}
		validBodyCount = 0;

		for (uint32_t i = 0; true; i++) {
			if (bodies.getIsValid(i) == false)
				continue;
			validBodyCount++;
			/*if (bodies[i].vel.isZero()) {
				if (validBodyCount >= bodyCount) break;
				continue;
			}*/

			static std::vector<BodyID> IDs;
			IDs.resize(0);
			spatialHashTable.getIDs(bodies[i].pos, bodies[i].siz, IDs);
			size_t IDCount = IDs.size();
			for (size_t j = 0; j < IDCount; j++) {
				if (i == IDs[j].id)
					continue;
				if (bodies[i].collidesWith(bodies[IDs[j].id])) {
					overlappingBodyPushIfUnique(i, IDs[j]);
					overlappingBodyPushIfUnique(IDs[j].id, { i });
				}
			}
			if (validBodyCount >= bodyCount) break;
		}
	}
	inline void resolve() {
		uint32_t bodyCount = bodies.getCount();
		if (bodyCount == 0) return;
		uint32_t validBodyCount = 0;
		for (uint32_t i = 0; true; i++) {
			if (bodies.getIsValid(i) == false)
				continue;
			validBodyCount++;
			if (overlappingBodyIDs[i].count && bodies[i].vel.isZero() == false && bodies[i].solid) {
				spatialHashTable.removeBody({ i }, bodies[i].pos, bodies[i].siz);
				bodies[i].reverseSimulate();
				spatialHashTable.addBody({ i }, bodies[i].pos, bodies[i].siz);
			}
			if (validBodyCount >= bodyCount) break;
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
};