#include <IO_API/IO_API.h>
#include <DDECS.h>
#include <PhysicsEngine.h>
//#include "PhysicsEngineConvex.h"
#include <EntityObjectLoader.h>
#include "Asset.h"
#include <iostream>
#include <time.h>
#include <stdio.h>

#include "systems/systems.h"

struct ThreadPool* threadPool;

void foo(void*) {

}

void spawnEntity(const char* fileName, uint32_t x, uint32_t y) {
	std::string fullPath = getDirData();
	fullPath += fileName;
	EntityObject entityObject = EntityObjectLoader::createEntityObjectFromFile(fullPath);
	EntityID entity = ecs.getNewEntity();
	for (ComponentObject& component : entityObject.components) {
		std::string name = component.name;
		uint32_t size = component.size;
		if (name == "size") {
			uint32_t* data = component.getArray();
			BodyID bodyID = physics.addBodyRect(x, y, data[0], data[1]);
			physics.setUserData(bodyID, (void*)entity);
			ecs.emplace(entity, bodyComponentID, &bodyID);
			continue;
		}
		if (name == "texture") {
			Name textureName = component.getString();
			std::string textureStr = getDirData(); textureStr += component.getString().c_str();
			TextureID tex = TextureCodex::add(textureStr);
			ecs.emplace(entity, textureComponentID, &tex);
			continue;
		}
		if (size > sizeof(uint32_t)) {
			void* data = component.getArray();
			ComponentID componentID = ecs.registerComponent(name, size);
			ecs.emplace(entity, componentID, data);
			continue;
		}
		if (size == 0) {
			ComponentID componentID = ecs.registerComponent(name, size);
			ecs.emplace(entity, componentID, NULL);
			continue;
		}
		uint32_t data = component.getInt();
		ComponentID componentID = ecs.registerComponent(name, size);
		ecs.emplace(entity, componentID, &data);
	}
}

void appStart() {
	srand(time(NULL));

	auto c = clock();

	std::cout << "Pre initialization finished in " << c << " miliseconds." << std::endl;
	std::cout << "Initializing app...    ";

	setFPS(60);

	physics.init();

	//threadPool = ThreadPoolCreate(8);
	//ThreadPoolGiveTask(threadPool, foo, NULL);

	registerComponents();
	systemAI.init();


	EntityID player = ecs.getNewEntity();
	BodyID bodyID = physics.addBodyRect(128, 128, 16, 16);
	physics.setUserData(bodyID, (void*)player);
	TextureID koiTexID = TextureCodex::add(std::string(getDirData())+"Textures/Koishi.png");
	ecs.emplace(player, bodyComponentID, &bodyID);
	ecs.emplace(player, playerControllerComponentID, NULL);
	ecs.emplace(player, textureComponentID, &koiTexID);
	ecs.emplace(player, detectOverlapComponentID, NULL);
	uint32_t health = 100;
	ecs.emplace(player, healthComponentID, &health);
	//ecs.emplace(player, suicideOnCollisionComponentID, NULL);

	for(auto i = 0; i < 1; i++)
		for (auto j = 0; j < 1; j++) {
			/*EntityID npc = ecs.getNewEntity();
			BodyID bodyID = physics.addBodyRect(i*96, j*96, 64, 64);
			physics.addVelocity(bodyID, 2*i%3, 2*i%5);
			physics.setUserData(bodyID, (void*)npc);
			ecs.emplace(npc, bodyComponentID, &bodyID);
			ecs.emplace(npc, textureComponentID, &koiTexID);
			ecs.emplace(npc, healthComponentID, &health);
			ecs.emplace(npc, detectOverlapComponentID, NULL);*/
			spawnEntity("Entities/Drone.txt", i * 96, j * 96);
		}
	c = clock()-c;
	std::cout << "Done!  \nInitialized in " << c << " miliseconds." << std::endl;
}
void appLoop() {
	clock_t c = clock();
	physics.tick();
	const char* ms = "ms";
	std::string entityCount = "Entities: ";
	entityCount += std::to_string((int)ecs.getEntityCount());
	std::string physicsTime = "Physics: ";
	physicsTime += std::to_string((int)physics.getTime()); physicsTime += ms;
	systemDisplay.run();
	systemPlayer.run();
	systemDamage.run();
	systemDead.run();
	systemAI.run();
	std::string displayTime = "Display: ";
	displayTime += systemDisplay.getTimeMSStr(); displayTime += ms;
	std::string playerTime = "Player: ";
	playerTime += systemDisplay.getTimeMSStr(); playerTime += ms;
	std::string damageTime = "Damage: ";
	damageTime += systemDisplay.getTimeMSStr(); damageTime += ms;
	std::string deadTime = "Dead: ";
	deadTime += systemDisplay.getTimeMSStr(); deadTime += ms;
	std::string AITime = "AI: ";
	AITime += systemDisplay.getTimeMSStr(); AITime += ms;
	std::string totalTime = "totalTime: ";
	totalTime += std::to_string((int)(clock() - c)); totalTime += ms;
	drawText(entityCount.c_str(), 16, 0, 12);
	drawText(physicsTime.c_str(), 16, 16, 12);
	drawText(displayTime.c_str(), 16, 32, 12);
	drawText(playerTime.c_str(), 16, 48, 12);
	drawText(damageTime.c_str(), 16, 64, 12);
	drawText(deadTime.c_str(), 16, 80, 12);
	drawText(totalTime.c_str(), 16, 96, 12);

	drawText(std::to_string(getFPS()).c_str(), 700, 0, 12);
}
void appEnd() {
	//ThreadPoolDestroy(threadPool);
}