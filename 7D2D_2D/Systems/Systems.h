#pragma once

#include <DDECS.h>
#include <PhysicsEngineAABB.h>
#include "Pathfinding.h"
#include "Voxel/Voxel.h"

extern DDECS<24, 100008> ecs;
extern PhysicsEngine<512 / 2, 512 / 2, 128> physics;
extern Pathfinding<512 * 2, 512 * 2, 32> pathfinding;
extern VoxelWorld voxelWorld;

#include "SystemDeath.h"
#include "SystemDisplay.h"
#include "SystemPhysics.h"
#include "SystemGetTexture.h"
#include "SystemDamage.h"
#include "SystemController.h"
#include "SystemQueSpawn.h"
#include "SystemMoveTo.h"
#include "SystemUnitAI.h"
#include "SystemDestructOnOverlap.h"
#include "SystemVoxelPhysics.h"

void initSystems() {
	ecs.registerSystem<SystemDeath>();
	ecs.registerSystem<SystemDisplay>();
	ecs.registerSystem<SystemPhysics>();
	ecs.registerSystem<SystemGetTexture>();
	ecs.registerSystem<SystemDamage>();
	ecs.registerSystem<SystemController>();
	ecs.registerSystem<SystemQueSpawn>();
	ecs.registerSystem<SystemMoveTo>();
	ecs.registerSystem<SystemUnitAI>();
	ecs.registerSystem<SystemDestructOnOverlap>();
	ecs.registerSystem<SystemVoxelPhysics>();
}