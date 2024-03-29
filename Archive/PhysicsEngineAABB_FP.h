#pragma once

#include "FlatBuffer.h"
//#include "FixedPoint.h"
#include <fpm/fixed.hpp>
#include <fpm/math.hpp>
#include <fpm/ios.hpp>
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
	__forceinline double getDistance() {
		return sqrt(x * x + y * y);
	}
	__forceinline uint64_t getDistanceSquared() {
		return x * x + y * y;
	}
	__forceinline double getDistanceFrom(Vec2D& other) {
		return (*this - other).getDistance();
	}
	__forceinline uint64_t getDistanceFromSquared(Vec2D& other) {
		return (*this - other).getDistanceSquared();
	}
};

struct BodyAABB {
	bool solid;
	Vec2D<fpm::fixed_24_8> pos, siz, vel;

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
	void addBody(BodyID id, const Vec2D<fpm::fixed_24_8>& pos, const Vec2D<fpm::fixed_24_8>& siz) {
		uint32_t startRight = pos.x / hashWidth, startDown = pos.y / hashWidth;
		uint32_t endRight = (pos.x + siz.x) / hashWidth, endDown = (pos.y + siz.y) / hashWidth;
		for (uint32_t y = startDown; y <= endDown; y++)
			for (uint32_t x = startRight; x <= endRight; x++)
				addBodyToHash(id, x + y * width);
	}
	void removeBody(BodyID id, const Vec2D<fpm::fixed_24_8>& pos, const Vec2D<fpm::fixed_24_8>& siz) {
		uint32_t startRight = pos.x / hashWidth, startDown = pos.y / hashWidth;
		uint32_t endRight = (pos.x + siz.x) / hashWidth, endDown = (pos.y + siz.y) / hashWidth;
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
	__forceinline std::vector< FlatBuffer<BodyID, max_bodies_per_hash>*> getHashes(Vec2D<fpm::fixed_24_8>& pos, Vec2D<fpm::fixed_24_8>& siz) {
		std::vector< FlatBuffer<BodyID, max_bodies_per_hash>*> returnValue;
		uint32_t startRight = pos.x / hashWidth, startDown = pos.y / hashWidth;
		uint32_t endRight = (pos.x + siz.x) / hashWidth, endDown = (pos.y + siz.y) / hashWidth;
		for (uint32_t y = startDown; y <= endDown; y++)
			for (uint32_t x = startRight; x <= endRight; x++)
				returnValue.push_back(&hash[x + y * width]);
		return returnValue;
	}
	// This was complicated and may need another look over
	void getIDs(const Vec2D<fpm::fixed_24_8>& pos, const Vec2D<fpm::fixed_24_8>& siz, std::vector<BodyID>& returnValue) {
		uint32_t startRight = (pos.x / hashWidth)., startDown = pos.y / hashWidth;
		uint32_t endRight = (pos.x + siz.x) / hashWidth, endDown = (pos.y + siz.y) / hashWidth;
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

class PhysicsEngine {
	static constexpr uint32_t max_bodies = 100000;
	//static constexpr uint32_t unit_size = 256;  //used as a pretend "1" normalized value to emulate decimal
	FlatFlaggedBuffer<BodyAABB, max_bodies> bodies = FlatFlaggedBuffer<BodyAABB, max_bodies>();
	FlatBuffer<void*, max_bodies> userData;
	FlatBuffer<FlatBuffer<BodyID, 100>, max_bodies> overlappingBodyIDs;
	//SpatialHashTable<300, 300, 128*unit_size> spatialHashTable;
	SpatialHashTable<2000, 2000, 128> spatialHashTable;
	float timeFromStepping = 0;
public:
	void init() {

	}
	BodyID addBodyRect(int32_t _x, int32_t _y, int32_t _w, int32_t _h) {
		fpm::fixed_24_8 x(_x), y(_y), w(_w), h(_h), vx(0), vy(0);
		BodyID retValue;
		BodyAABB body = { true, { x,y }, { w,h }, { vx,vy } };
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

	inline std::vector<BodyID> getBodiesInRectRough(Vec2D<fpm::fixed_24_8>& pos, Vec2D<fpm::fixed_24_8>& siz) {
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

	/*void addVelocity(BodyID id, float vx, float vy) {
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
	}*/
	void addVelocity(BodyID id, fpm::fixed_24_8 vx, fpm::fixed_24_8 vy) {
		BodyAABB* body = &bodies[id.id];
		body->vel.x += vx;
		body->vel.y += vy;
	}
	void setVelocity(BodyID id, fpm::fixed_24_8 vx, fpm::fixed_24_8 vy) {
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

	Vec2D<fpm::fixed_24_8> getPos(BodyID id) {
		Vec2D<fpm::fixed_24_8> bodyPos = bodies[id.id].pos;
		return bodyPos;
	}
	Vec2D<fpm::fixed_24_8> getSize(BodyID id) {
		Vec2D<fpm::fixed_24_8> bodySiz = bodies[id.id].siz;
		return bodySiz;
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