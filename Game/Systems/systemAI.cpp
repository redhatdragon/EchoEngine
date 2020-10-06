#include "systemAI.h"
#include "systems.h"

struct Path {
	FlatBuffer<Vec2D<int32_t>, 64> points;
	uint8_t currentIndex;
};

struct AI {
	uint8_t team : 4;
	uint8_t importance : 4;
	enum class Target {
		NONE,
		PLAYER,
		ALLY,
		ENEMY
	} target : 2;
	enum class Action {
		NONE,
		ATTACK,
		DEFEND
	} action : 2;
};

void systemAI() {
	AI* AIComponent = (AI*)ecs.getComponentBuffer(AIComponentID);
	uint32_t AICount = ecs.getComponentCount(AIComponentID);
	for (uint32_t i = 0; i < AICount; i++) {
		static constexpr uint32_t chance = 33;  //In percent
		if (rand() % 100 > chance) continue;
	}
}