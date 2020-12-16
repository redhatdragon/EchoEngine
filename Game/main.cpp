#include <IO_API/IO_API.h>
#include <DDECS.h>
#include <PhysicsEngine.h>
//#include "PhysicsEngineConvex.h"
#include "Asset.h"
#include <iostream>
#include <time.h>
#include <stdio.h>

#include "systems/systems.h"

struct ThreadPool* threadPool;

void foo(void*) {

}

void appStart() {
	srand(time(NULL));

	auto c = clock();

	std::cout << "Pre initialization finished in " << c << " miliseconds." << std::endl;
	std::cout << "Initializing app...    ";

	setFPS(120);

	physics.init();

	//threadPool = ThreadPoolCreate(8);
	//ThreadPoolGiveTask(threadPool, foo, NULL);

	registerComponents();


	EntityID player = ecs.getNewEntity();
	BodyID bodyID = physics.addBodyRect(128, 128, 16, 16);
	physics.setUserData(bodyID, (void*)player);
	TextureID koiTexID = TextureCodex::add(std::string(getDirData())+"textures/Koishi.png");
	ecs.emplace(player, bodyComponentID, &bodyID);
	ecs.emplace(player, playerControllerComponentID, NULL);
	ecs.emplace(player, textureComponentID, &koiTexID);
	ecs.emplace(player, detectOverlapComponentID, NULL);
	uint32_t health = 100;
	ecs.emplace(player, healthComponentID, &health);
	//ecs.emplace(player, suicideOnCollisionComponentID, NULL);

	for(auto i = 0; i < 40000; i++)
		for (auto j = 0; j < 1; j++) {
			EntityID npc = ecs.getNewEntity();
			BodyID bodyID = physics.addBodyRect(i*96, j*96, 64, 64);
			physics.addVelocity(bodyID, 2*i%3, 2*i%5);
			physics.setUserData(bodyID, (void*)npc);
			ecs.emplace(npc, bodyComponentID, &bodyID);
			ecs.emplace(npc, textureComponentID, &koiTexID);
			ecs.emplace(npc, healthComponentID, &health);
			ecs.emplace(npc, detectOverlapComponentID, NULL);
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
	std::string displayTime = "Display: ";
	displayTime += systemDisplay.getTimeMSStr(); displayTime += ms;
	std::string playerTime = "Player: ";
	playerTime += systemDisplay.getTimeMSStr(); playerTime += ms;
	std::string damageTime = "Damage: ";
	damageTime += systemDisplay.getTimeMSStr(); damageTime += ms;
	std::string deadTime = "Dead: ";
	deadTime += systemDisplay.getTimeMSStr(); deadTime += ms;
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