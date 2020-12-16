#include "systems.h"
#include "systemAI.h"

void SystemAI::run() {
	clock_t c = clock();
	AI* AIComponent = (AI*)ecs.getComponentBuffer(AIComponentID);
	uint32_t AICount = ecs.getComponentCount(AIComponentID);
	for (uint32_t i = 0; i < AICount; i++) {
		static constexpr uint32_t chance = 33;  //In percent
		if (rand() % 100 > chance) continue;
	}
	ms = clock() - c;
}