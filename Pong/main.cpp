#include <IO_API/IO_API.h>
#include <DDECS.h>
#include <PhysicsEngine.h>
//#include "PhysicsEngineConvex.h"
#include "Asset.h"
#include <iostream>
#include <time.h>

#include "systems/systems.h"

struct ThreadPool* threadPool;

void foo(void*) {

}

void appStart() {
	auto c = clock();

	std::cout << "Pre initialization finished in " << c << " miliseconds." << std::endl;
	std::cout << "Initializing app...    ";

	physics.init();

	//threadPool = ThreadPoolCreate(8);
	//ThreadPoolGiveTask(threadPool, foo, NULL);

	registerComponents();


	EntityID player = ecs.getNewEntity();
	BodyID bodyID = physics.addBodyRect(128, 128, 16, 16);
	physics.setUserData(bodyID, (void*)player);
	TextureID koiTexID = TextureCodex::add("data/Textures/Koishi.png");
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
	physics.tick();
	systemDisplay();
	systemPlayer();
	systemDamage();
	systemDead();
}
void appEnd() {
	//ThreadPoolDestroy(threadPool);
}