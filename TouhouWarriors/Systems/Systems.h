#pragma once

#include <DDECS.h>
#include <PhysicsEngineAABB.h>

extern DDECS<24, 100000> ecs;
extern PhysicsEngine physics;

void destructBody(void* componentData) {
	BodyID bodyID = *(BodyID*)componentData;
	physics.removeBody(bodyID);
}

void registerComponentDestructors() {
	ComponentID componentBodyID = ecs.getComponentID("body");
	ecs.registerDestructor(componentBodyID, destructBody);
}

#include "SystemDeath.h"

void initSystems() {
	ecs.registerSystem<SystemDeath>();
}