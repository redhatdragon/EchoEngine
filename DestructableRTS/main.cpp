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
PhysicsEngine physics;



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



struct ShipPart {
	uint16_t value;
	uint8_t type;
};
struct ShipPartBuffers {
	DArray<ShipPart> parts;
	// Cached data
	//DArray<uint16_t> armorPartIndices;
	std::vector<uint32_t> t;
	DArray<uint16_t> powerPartIndices;
	DArray<uint16_t> thrustPartIndices;
	DArray<uint16_t> partColors;
	void* texturePtr;
	void init() {
		constexpr int MAX_NUM_PARTS = 256 * 256;
		parts.init(MAX_NUM_PARTS);
		powerPartIndices.init(MAX_NUM_PARTS);
		thrustPartIndices.init(MAX_NUM_PARTS);
	}
	void destruct() {
		parts.free();
		powerPartIndices.free();
		thrustPartIndices.free();
	}
};
struct ShipData {
	uint32_t health, shield, crew;
	uint16_t maxSpeed;
	uint8_t w, h;
private:
	ShipPartBuffers* shipPartBuffers;
public:
	void init() {
		shipPartBuffers = (ShipPartBuffers*)malloc(sizeof(ShipPartBuffers));
		shipPartBuffers->init();
	}
	void destruct() {
		shipPartBuffers->destruct();
		free(shipPartBuffers);
	}
	inline ShipPart& getShipPart(uint16_t index) {
		return shipPartBuffers->parts[index];
	}
};



#define MAX_SHIPS 100000




void appStart() {
	profileLinesStart();
	initSystems();
	profileLinesEnd();

	SystemUtilities::spawnEntityAtWithSize("Entities/Killzone.txt", { 0, 0 }, { 2000, 20 });
	SystemUtilities::spawnEntityAtWithSize("Entities/Killzone.txt", { 0, 0 }, { 20, 2000 });
	SystemUtilities::spawnEntityAtWithSize("Entities/Killzone.txt", { 2000-20, 0 }, { 20, 2000 });
	SystemUtilities::spawnEntityAtWithSize("Entities/Killzone.txt", { 0, 2000-20 }, { 2000, 20 });

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
}

void appEnd() {

}