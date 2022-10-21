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

constexpr uint32_t sizeOfECS = sizeof(ecs);
constexpr uint32_t sizeOfPhysics = sizeof(physics);
constexpr uint32_t sizeOfPathfinding = sizeof(pathfinding);
constexpr uint32_t bytesUsed = sizeOfECS + sizeOfPhysics + sizeOfPathfinding;



bool componentExistsErrorCheck(EntityObject& eo, const std::string& componentName, const std::string& errorLocation,
		ComponentObject::TYPE type, uint32_t arraySize = 0) {
	ComponentObject* c = eo.getComponent(componentName);
	if (c == nullptr) {
		std::string errorMsg = "Error: ";
		errorMsg += errorLocation;
		errorMsg += " didn't have a ";
		errorMsg += componentName;
		errorMsg += " component.";
		std::cout << errorMsg << std::endl;
		return false;
	}
	if (c->type != type) {
		std::string errorMsg = "Error: ";
		errorMsg += errorLocation;
		errorMsg += " ";
		errorMsg += componentName;
		errorMsg += " component isn't of type ";
		errorMsg += c->typeAsString();
		std::cout << errorMsg << std::endl;
		return false;
	}
	if (c->type == ComponentObject::TYPE::TYPE_ARRAY) {
		if (c->getArrayIntLen() != arraySize) {
			std::string errorMsg = "Error: ";
			errorMsg += errorLocation;
			errorMsg += " ";
			errorMsg += componentName;
			errorMsg += " oes not have correct number of elements.";
			std::cout << errorMsg << std::endl;
			return false;
		}
	}
	return true;
}

EntityID buildFactory(uint32_t x, uint32_t y, uint32_t team, const std::string& factoryFilepath) {
	ComponentID bodyComponentID = ecs.registerComponent("body", sizeof(BodyID));
	std::string dataDir = getDirData();

	EntityObject entityObject = EntityObjectLoader::createEntityObjectFromFile(dataDir + factoryFilepath);
	EntityID entity = ecs.getNewEntity();
	if (componentExistsErrorCheck(entityObject, "size", "buildFactory()'s factoryFilepath's", 
			ComponentObject::TYPE::TYPE_ARRAY, 2) == false)
		return -1;
	ComponentObject* size = entityObject.getComponent("size");
	BodyID bodyID = physics.addBodyRect(x, y, size->getArray()[0], size->getArray()[1]);
	physics.setUserData(bodyID, (void*)entity);
	physics.setSolid(bodyID, false);
	ecs.emplace(entity, bodyComponentID, &bodyID);
	if (componentExistsErrorCheck(entityObject, "queSpawn", "buildFactory()'s factoryFilepath's",
		ComponentObject::TYPE::TYPE_ARRAY, 2) == false) {
		ecs.removeEntity(entity);
		return -1;
	}
	ComponentObject* health = entityObject.getComponent("health");
	if (componentExistsErrorCheck(entityObject, "health", "buildFactory()'s factoryFilepath's",
		ComponentObject::TYPE::TYPE_ARRAY, 6) == false) {
		ecs.removeEntity(entity);
		return -1;
	}
	auto healthBuffer = health->getArray();
	healthBuffer[0] = team;
	ComponentObject* queSpawn = entityObject.getComponent("queSpawn");
	{
		uint32_t* queSpawnArr= queSpawn->getArray();
		queSpawnArr[0] = x + size->getArray()[0];
		queSpawnArr[1] = y + size->getArray()[1];
	}
	if (entityObject.getComponent("queWaypoint") != nullptr) {
		std::cout << "Error: BuildFactory()'s factoryFilepath has queWaypoint when it shouldn't." << std::endl;
		ecs.removeEntity(entity);
		return -1;
	}
	Vec2D<int32_t> queWaypoint;
	memcpy(&queWaypoint, queSpawn->getArray(), sizeof(Vec2D<int32_t>));
	queWaypoint += Vec2D<int32_t>{(int32_t)x,(int32_t)y};
	ComponentID queWaypointComponentID = ecs.registerComponent("queWaypoint", sizeof(Vec2D<int32_t>));
	ecs.emplace(entity, queWaypointComponentID, &queWaypoint);
	for (auto& c : entityObject.components) {
		if (c.name == "size") continue;
		ComponentID componentID = ecs.registerComponent(c.name, c.size);
		if (c.type == ComponentObject::TYPE::TYPE_INT ||
			c.type == ComponentObject::TYPE::TYPE_FIXED_FLOAT ||
			c.type == ComponentObject::TYPE::TYPE_STRING ||
			c.type == ComponentObject::TYPE::TYPE_DARRAY)
			ecs.emplace(entity, componentID, &c.data);
		else if (c.type == ComponentObject::TYPE::TYPE_ARRAY)
			ecs.emplace(entity, componentID, c.data);
		else if (c.type == ComponentObject::TYPE::TYPE_NULL)
			ecs.emplace(entity, componentID, NULL);
		else {
			std::cout << "error: buildFactory()'s factoryFilepath obtained component of invalid type?" << std::endl;
			return -1;
		}
	}
	ComponentID queTickComponentID = ecs.registerComponent("queTick", sizeof(uint32_t));
	uint32_t queTick = 0;
	ecs.emplace(entity, queTickComponentID, &queTick);
	ComponentID queComponentID = ecs.registerComponent("que", sizeof(FlatBuffer<Name, 5>));
	FlatBuffer<Name, 5> que;
	ecs.emplace(entity, queComponentID, &que);
	return entity;
}

void setFactoryWaypoint(EntityID entity, int32_t x, int32_t y) {
	ComponentID queWaypointComponentID = ecs.getComponentID("queWaypoint");
	if (queWaypointComponentID == -1) {
		std::cout << "Error: setFactoryWaypoint(); queWaypoint component isn't yet registered." <<
			std::endl << "Was there any factories made yet?" << std::endl;
		return;
	}
	Vec2D<int32_t>* queWaypoint = (Vec2D<int32_t>*)ecs.getEntityComponent(entity, queWaypointComponentID);
	if (queWaypoint == nullptr) {
		std::cout << "Error: setFactoryWaypoint()'s Entity isn't valid; missing queWaypoint." << std::endl;
		return;
	}
	*queWaypoint = Vec2D<int32_t>{ x, y };
}

void addQueToFactory(EntityID entity, Name quePath) {

	ComponentID queComponentID = ecs.getComponentID("que");
	if (queComponentID == -1) {
		std::cout << "Error: addQueToFactory(); que component isn't yet registered." <<
			std::endl << "Was there any factories made yet?" << std::endl;
		return;
	}
	FlatBuffer<Name, 5>* que = (FlatBuffer<Name, 5>*)ecs.getEntityComponent(entity, queComponentID);
	if (que == nullptr) {
		std::cout << "Error: addQueToFactory()'s Entity isn't valid; missing que." << std::endl;
		return;
	}
	if(que->count < 5)
		que->push(quePath);
}



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



void stressTesting2() {
	SystemUtilities::spawnEntityAtWithSize("Entities/Killzone.txt", { 0, 0 }, { 2000, 20 });
	SystemUtilities::spawnEntityAtWithSize("Entities/Killzone.txt", { 0, 0 }, { 20, 2000 });
	SystemUtilities::spawnEntityAtWithSize("Entities/Killzone.txt", { 2000 - 20, 0 }, { 20, 2000 });
	SystemUtilities::spawnEntityAtWithSize("Entities/Killzone.txt", { 0, 2000 - 20 }, { 2000, 20 });

	for (uint32_t i = 0; i < 1000; i++) {
		EntityID factory1 = buildFactory(64, 256 + i * 52, 1, "Entities/Factory.txt");
		EntityID factory2 = buildFactory(512, 256 + i * 52, 2, "Entities/Factory.txt");
		setFactoryWaypoint(factory1, (512 - 64) / 2, 256);
		setFactoryWaypoint(factory2, (512 - 64) / 2, 256);
		addQueToFactory(factory1, "solder");
		addQueToFactory(factory1, "solder");
		addQueToFactory(factory2, "solder");
		addQueToFactory(factory2, "solder");
	}
}



void appStart() {
	profileLinesStart();
	initSystems();
	profileLinesEnd();
	//stressTesting1();
	testFixedPoint();
	testVec2D();
	//return;

	stressTesting2();

	/*EntityID factory1 = buildFactory(64, 256, 1, "Entities/Factory.txt");
	EntityID factory2 = buildFactory(512, 256, 2, "Entities/Factory.txt");
	setFactoryWaypoint(factory1, (512-64)/2, 256);
	setFactoryWaypoint(factory2, (512-64)/2, 256);
	addQueToFactory(factory1, "solder");
	addQueToFactory(factory2, "solder");
	addQueToFactory(factory2, "solder");
	addQueToFactory(factory2, "solder");*/

	/*EntityID camEntity = ecs.getNewEntity();
	unsigned int width, height;
	getCanvasSize(&width, &height);
	Name path = "Textures/Koishi.png";
	ComponentID cameraComponentID = ecs.registerComponent("camera", NULL);
	ComponentID getTextureComponentID = ecs.registerComponent("getTexture", sizeof(Name));
	ComponentID bodyComponentID = ecs.registerComponent("body", sizeof(BodyID));
	ComponentID controllerComponentID = ecs.registerComponent("controller", NULL);
	ecs.emplace(camEntity, cameraComponentID, nullptr);
	ecs.emplace(camEntity, getTextureComponentID, &path);
	BodyID bodyID = physics.addBodyRect(width / 2-200, height / 2, (uint32_t)8, (uint32_t)8);
	physics.setSolid(bodyID, false);
	ecs.emplace(camEntity, bodyComponentID, &bodyID);
	ecs.emplace(camEntity, controllerComponentID, nullptr);
	*/
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