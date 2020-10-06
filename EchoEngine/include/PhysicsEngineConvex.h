#pragma once

#pragma comment(lib, "chipmunk.lib")

#include <chipmunk/chipmunk.h>
#include <FlatBuffer.h>
#include <iostream>
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
	__forceinline void operator/=(T& num) {
		x /= num;
		y /= num;
	}
};

struct BodyID {
	//uint32_t id;
	cpBody* id;
};




class PhysicsEngine {
	static constexpr uint32_t max_bodies = 100000;
	static constexpr uint32_t unit_size = 1000;
	cpSpace* space;
	std::vector<cpBody*> debug;
	float timeFromStepping;
	//FlatFlaggedBuffer<cpBody*, max_bodies> bodies;
	//FlatBuffer<Vec2D<uint32_t>, max_bodies> bodySizes;
	
public:
	void init() {
		space = cpSpaceNew();
		cpSpaceSetIterations(space, 1);
		cpSpaceUseSpatialHash(space, 64, 30000);
	}
	void destroy() {
		cpSpaceFree(space);
	}
	BodyID addBodyRect(int32_t x, int32_t y, int32_t w, int32_t h, double mass = 1, double friction = 0.5) {
		cpBody* body = cpSpaceAddBody(space, cpBodyNew(mass, cpMomentForBox(mass, w, h)));
		cpShape* rect = cpSpaceAddShape(space, cpBoxShapeNew(body, w, h, 0.0));
		cpShapeSetFriction(rect, friction);
		cpShapeSetElasticity(rect, 1.0f);
		cpBodySetPosition(body, { (cpFloat)x, (cpFloat)y });

		//cpBody* body = cpSpaceAddBody(space, cpBodyNew(1.0f, cpMomentForBox(1.0f, 30.0f, 30.0f)));
		//cpBodySetPosition(body, cpv(j * 32 - i * 16, 300 - i * 32));

		//cpShape* shape = cpSpaceAddShape(space, cpBoxShapeNew(body, 30.0f, 30.0f, 0.5f));
		//cpShapeSetElasticity(shape, 0.0f);
		//cpShapeSetFriction(shape, 0.8f);

		debug.push_back(body);
		return { body };
	}
	/*BodyID addBody(float mass) {
		cpBody* body = cpBodyNew(mass, 0);
		cpSpaceAddBody(space, body);
	}*/

	void* getUserData(BodyID id) {
		return cpBodyGetUserData(id.id);
	}
	void setUserData(BodyID id, void* data) {
		cpBodySetUserData(id.id, data);
	}

	FlatBuffer<BodyID, 100>& getOverlappingBodies(BodyID id) {
		
	}

	void addVelocity(BodyID id, float vx, float vy) {
		cpVect vel = cpBodyGetVelocity(id.id);
		vel.x += vx; vel.y += vy;
		cpBodySetVelocity(id.id, vel);
	}
	void setVelocity(BodyID id, float vx, float vy) {
		cpVect vel = { vx, vy };
		cpBodySetVelocity(id.id, vel);
	}

	template<typename T>
	Vec2D<T> getPos(BodyID id) {
		cpVect pos = cpBodyGetPosition(id.id);
		return { (T)pos.x, (T)pos.y };
	}
	/*template<typename T>
	Vec2D<T> getSize(BodyID id) {
		
	}*/

	float getTime() {
		return timeFromStepping;
	}

	void tick() {
		clock_t c = clock();
		cpSpaceStep(space, 1);
		timeFromStepping = clock() - c;
	}
};