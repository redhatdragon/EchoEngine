#pragma once

#include "FlatBuffer.h"
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
	__forceinline double getDistanceFrom(Vec2D& other) {
		return (*this - other).getDistance();
	}
};

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
	void addBody(BodyID id, const Vec2D<int32_t>& pos, const Vec2D<int32_t>& siz) {
		uint32_t startRight = pos.x / hashWidth, startDown = pos.y / hashWidth;
		uint32_t endRight = (pos.x + siz.x) / hashWidth, endDown = (pos.y + siz.y) / hashWidth;
		for (uint32_t y = startDown; y <= endDown; y++)
			for (uint32_t x = startRight; x <= endRight; x++)
				addBodyToHash(id, x + y * width);
	}
	void removeBody(BodyID id, const Vec2D<int32_t>& pos, const Vec2D<int32_t>& siz) {
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
	__forceinline std::vector< FlatBuffer<BodyID, max_bodies_per_hash>*> getHashes(Vec2D<int32_t>& pos, Vec2D<int32_t>& siz) {
		std::vector< FlatBuffer<BodyID, max_bodies_per_hash>*> returnValue;
		uint32_t startRight = pos.x / hashWidth, startDown = pos.y / hashWidth;
		uint32_t endRight = (pos.x + siz.x) / hashWidth, endDown = (pos.y + siz.y) / hashWidth;
		for (uint32_t y = startDown; y <= endDown; y++)
			for (uint32_t x = startRight; x <= endRight; x++)
				returnValue.push_back(&hash[x + y * width]);
		return returnValue;
	}
	// This was complicated and may need another look over
	void getIDs(const Vec2D<int32_t>& pos, const Vec2D<int32_t>& siz, std::vector<BodyID>& returnValue) {
		uint32_t startRight = pos.x / hashWidth, startDown = pos.y / hashWidth;
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
	static constexpr uint32_t unit_size = 256;  //used as a pretend "1" normalized value to emulate decimal
	FlatFlaggedBuffer<BodyAABB, max_bodies> bodies1 = FlatFlaggedBuffer<BodyAABB, max_bodies>();
	FlatFlaggedBuffer<BodyAABB, max_bodies> bodies2 = FlatFlaggedBuffer<BodyAABB, max_bodies>();
	FlatFlaggedBuffer<BodyAABB, max_bodies>* bodiesFront = &bodies1;
	FlatFlaggedBuffer<BodyAABB, max_bodies>* bodiesBack = &bodies2;
	FlatBuffer<void*, max_bodies> userData;
	FlatBuffer<FlatBuffer<BodyID, 100>, max_bodies> overlappingBodyIDs;
	//SpatialHashTable<300, 300, 128*unit_size> spatialHashTable;
	SpatialHashTable<2000, 2000, 128 * unit_size> spatialHashTable;
	float timeFromStepping = 0;
public:
	void init() {

	}
	BodyID addBodyRect(int32_t x, int32_t y, int32_t w, int32_t h) {
		x *= unit_size; y *= unit_size;
		w *= unit_size; h *= unit_size;
		BodyID retValue;
		BodyAABB body = { true, { x,y }, { w,h }, { 0,0 } };
		retValue.id = bodiesFront->insert(body);
		spatialHashTable.addBody(retValue, body.pos, body.siz);
		overlappingBodyIDs[retValue.id].clear();
		return retValue;
	}
	void removeBody(BodyID bodyID) {
		BodyAABB& body = (*bodiesFront)[bodyID.id];
		spatialHashTable.removeBody(bodyID, body.pos, body.siz);
		bodiesFront->decrementCount();
		bodiesFront->setInvalid(bodyID.id);
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
				for (auto e : retValue) {
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
		BodyAABB* body = &(*bodiesFront)[id.id];
		body->vel.x += vx * unit_size;
		body->vel.y += vy * unit_size;
	}
	void setVelocity(BodyID id, float vx, float vy) {
		BodyAABB* body = &(*bodiesFront)[id.id];
		body->vel.x = vx * unit_size;
		body->vel.y = vy * unit_size;
	}

	bool getSolid(BodyID id) {
		return (*bodiesFront)[id.id].solid;
	}
	void setSolid(BodyID id, bool isTrue) {
		(*bodiesFront)[id.id].solid = isTrue;
	}

	template<typename T>
	Vec2D<T> getPos(BodyID id) {
		Vec2D<int32_t> bodyPos = (*bodiesFront)[id.id].pos;
		Vec2D<T> pos = { bodyPos.x / unit_size, bodyPos.y / unit_size };
		return pos;
	}
	template<typename T>
	Vec2D<T> getSize(BodyID id) {
		Vec2D<int32_t> bodySiz = (*bodiesFront)[id.id].siz;
		Vec2D<T> siz = { bodySiz.x / unit_size, bodySiz.y / unit_size };
		return siz;
	}

	float getTime() {
		return timeFromStepping;
	}

	inline void simulate() {
		uint32_t bodyCount = bodiesFront->getCount();
		uint32_t validBodyCount = 0;
		for (uint32_t i = 0; true; i++) {
			if (bodiesFront->getIsValid(i) == false)
				continue;
			validBodyCount++;
			if ((*bodiesFront)[i].vel.isZero()) {
				if (validBodyCount >= bodyCount) break;
				continue;
			}
			spatialHashTable.removeBody({ i }, (*bodiesFront)[i].pos, (*bodiesFront)[i].siz);
			(*bodiesFront)[i].simulate();
			spatialHashTable.addBody({ i }, (*bodiesFront)[i].pos, (*bodiesFront)[i].siz);
			if (validBodyCount >= bodyCount) break;
		}
	}
	inline void detect() {
		uint32_t bodyCount = bodiesFront->getCount();
		uint32_t validBodyCount = 0;
		for (uint32_t i = 0; true; i++) {
			if (bodiesFront->getIsValid(i) == false)
				continue;
			validBodyCount++;
			overlappingBodyIDs[i].clear();
			if (validBodyCount >= bodyCount) break;
		}
		validBodyCount = 0;

		for (uint32_t i = 0; true; i++) {
			if (bodiesFront->getIsValid(i) == false)
				continue;
			validBodyCount++;
			/*if (bodies[i].vel.isZero()) {
				if (validBodyCount >= bodyCount) break;
				continue;
			}*/

			static std::vector<BodyID> IDs;
			IDs.resize(0);
			spatialHashTable.getIDs((*bodiesFront)[i].pos, (*bodiesFront)[i].siz, IDs);
			size_t IDCount = IDs.size();
			for (size_t j = 0; j < IDCount; j++) {
				if (i == IDs[j].id)
					continue;
				if ((*bodiesFront)[i].collidesWith((*bodiesFront)[IDs[j].id])) {
					overlappingBodyPushIfUnique(i, IDs[j]);
					overlappingBodyPushIfUnique(IDs[j].id, { i });
				}
			}
			if (validBodyCount >= bodyCount) break;
		}
	}
	inline void resolve() {
		uint32_t bodyCount = bodiesFront->getCount();
		uint32_t validBodyCount = 0;
		for (uint32_t i = 0; true; i++) {
			if (bodiesFront->getIsValid(i) == false)
				continue;
			validBodyCount++;
			if (overlappingBodyIDs[i].count && (*bodiesFront)[i].vel.isZero() == false && (*bodiesFront)[i].solid) {
				spatialHashTable.removeBody({ i }, (*bodiesFront)[i].pos, (*bodiesFront)[i].siz);
				(*bodiesFront)[i].reverseSimulate();
				spatialHashTable.addBody({ i }, (*bodiesFront)[i].pos, (*bodiesFront)[i].siz);
			}
			if (validBodyCount >= bodyCount) break;
		}
	}

	void tick() {
		clock_t c = clock();
		simulate();
		//swapBodyBuffers();
		swapBodyBuffersThreaded();
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
	void swapBodyBuffers() {
		memcpy(bodiesFront, bodiesBack, sizeof((*bodiesFront)));
		auto tempPtr = bodiesFront;
		bodiesFront = bodiesBack;
		bodiesBack = tempPtr;
	}
	void swapBodyBuffersThreaded() {
		struct tData {
			PhysicsEngine* self;
			uint32_t start;
			uint32_t count;
		};
		uint32_t threadCount = ThreadPoolGetFreeThreadCount(threadPool);
		uint32_t chunkCount = sizeof(*bodiesFront)/threadCount;
		struct tData* tdataBuffer = (struct tData*)malloc(sizeof(struct tData) * threadCount);
		for (uint32_t i = 0; i < threadCount; i++) {
			//struct tData* tdata = malloc(sizeof(tData));
			tdataBuffer[i].self = this; tdataBuffer[i].start = i * chunkCount; tdataBuffer[i].count = chunkCount;
			ThreadPoolGiveTask(threadPool, _swapBodyBuffersThreadedCallback, &tdataBuffer[i]);
		}
		uint8_t* front = (uint8_t*)bodiesFront;
		uint8_t* back = (uint8_t*)bodiesBack;
		memcpy(front+chunkCount*threadCount, bodiesBack+chunkCount*threadCount, sizeof(*bodiesFront) % threadCount);
		while (ThreadPoolGetFreeThreadCount(threadPool) != threadCount)
			continue;
		free(tdataBuffer);
		auto tempPtr = bodiesFront;
		bodiesFront = bodiesBack;
		bodiesBack = tempPtr;
	}
	static void _swapBodyBuffersThreadedCallback(void* _tdata) {
		struct tData {
			PhysicsEngine* self;
			uint32_t start;
			uint32_t count;
		};
		struct tData* tdata = (struct tData*)_tdata;
		uint8_t* front = (uint8_t*)tdata->self->bodiesFront;
		uint8_t* back = (uint8_t*)tdata->self->bodiesBack;
		memcpy(front+tdata->start, back+tdata->start, tdata->count);
		//free(tdata);
	}
};