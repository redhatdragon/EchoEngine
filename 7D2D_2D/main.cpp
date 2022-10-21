#pragma once

#include <IO_API/IO_API.h>
#include "Systems/Systems.h"
#include <profiler.h>
#include <Asset.h>
#include <iostream>
#include "Systems//SystemUtilities.h"
#include  <FixedPoint.h>
#include <EntityObjectLoader.h>

DDECS<24, 100008> ecs;
//PhysicsEngine<2000, 2000, 128> physics;
PhysicsEngine<512/2, 512/2, 128> physics;
Pathfinding<512*2, 512*2, 32> pathfinding;
VoxelWorld voxelWorld;

constexpr uint32_t sizeOfECS = sizeof(ecs);
constexpr uint32_t sizeOfPhysics = sizeof(physics);
constexpr uint32_t sizeOfPathfinding = sizeof(pathfinding);
constexpr uint32_t bytesUsed = sizeOfECS + sizeOfPhysics + sizeOfPathfinding;



void stressTesting1() {
	testFlatFlaggedBuffer();
	testFixedPoint();
	ComponentID bodyComponentID = ecs.registerComponent("body", sizeof(BodyID));
	ComponentID textureComponentID = ecs.registerComponent("texture", sizeof(TextureID));
	ComponentID healthComponentID = ecs.registerComponent("health", sizeof(SystemUtilities::Health));
	ComponentID damageOnCollisionComponentID = ecs.registerComponent("damageOnCollision", sizeof(SystemUtilities::DamageOnCollision));
	ComponentID controllerComponentID = ecs.registerComponent("controller", NULL);
	std::string koishiFilepath = getDirData();
	koishiFilepath += "/Textures/Koishi.png";
	for (uint32_t i = 0; i < 40000; i++) {
		EntityID entity = ecs.getNewEntity();
		BodyID bodyID = physics.addBodyRect(i * 96, (uint32_t)96, (uint32_t)64, (uint32_t)64);
		physics.addVelocity(bodyID, 2 * i % 3, 2 * i % 5);
		physics.setUserData(bodyID, (void*)entity);
		ecs.emplace(entity, bodyComponentID, &bodyID);
		TextureID koiTexture = TextureCodex::add(koishiFilepath);
		ecs.emplace(entity, textureComponentID, &koiTexture);
		SystemUtilities::Health health = { 1, 500.0f, {0.0f, .5f, .5f, .5f} };
		ecs.emplace(entity, healthComponentID, &health);
	}
	{
		EntityID entity = ecs.getNewEntity();
		BodyID bodyID = physics.addBodyRect((uint32_t)96 * 3, (uint32_t)96 * 5, (uint32_t)64, (uint32_t)64);
		physics.setUserData(bodyID, (void*)entity);
		ecs.emplace(entity, bodyComponentID, &bodyID);
		TextureID koiTexture = TextureCodex::add(koishiFilepath);
		ecs.emplace(entity, textureComponentID, &koiTexture);
		ComponentID cameraComponentID = ecs.registerComponent("camera", NULL);
		ecs.emplace(entity, cameraComponentID, NULL);
		SystemUtilities::DamageOnCollision dmg = { 0, 50.0f, {0.0f, 0.0f, 0.0f, 0.1f} };
		ecs.emplace(entity, damageOnCollisionComponentID, &dmg);
		ecs.emplace(entity, controllerComponentID, NULL);
	}
}



void appStart() {
	profileLinesStart();
	initSystems();
	profileLinesEnd();
	//stressTesting1();
	//testFixedPoint();
	//testVec2D();
	//return;



	EntityID camEntity = ecs.getNewEntity();
	unsigned int width, height;
	getCanvasSize(&width, &height);
	SystemUtilities::spawnEntityAt("Entities/Player.txt", { width / 2 - 200, height / 2 });
}

void appLoop() {
	ecs.runSystems();
	auto systemsDebugInfo = ecs.getDebugInfo();
	int x = 16, y = 16, w = 16;
	std::string entityCountStr = "EntityCount->";
	entityCountStr += std::to_string(ecs.getEntityCount());
	drawText(entityCountStr.c_str(), x, y, w);
	y += w + w / 2;
	for (auto& di : systemsDebugInfo) {
		drawText(di.c_str(), x, y, w);
		y += w + w / 2;
	}
	//drawText(std::to_string(TextureCodex::refCount[1]).c_str(), 600, 200, 16);
}

void appEnd() {

}