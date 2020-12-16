#pragma once
#include <IO_API/IO_API.h>
#include <DDECS.h>
#include <PhysicsEngine.h>
//#include "PhysicsEngineConvex.h"
#include "../Asset.h"
#include <iostream>
#include <time.h>

extern DDECS<24, 100000> ecs;
extern PhysicsEngine physics;

extern ComponentID bodyComponentID;
extern ComponentID playerControllerComponentID;
extern ComponentID textureComponentID;
extern ComponentID detectOverlapComponentID;
extern ComponentID damageOnCollisionComponentID;
extern ComponentID healthComponentID;
extern ComponentID deadComponentID;
extern ComponentID suicideOnCollisionComponentID;
extern ComponentID AIComponentID;

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

void registerComponents();

class System {
protected:
	clock_t ms = 0;  //must be updated in every run method
public:
	virtual void run() = 0;
	inline clock_t getTimeMS() const {
		return ms;
	}
	std::string getTimeMSStr() const {
		return std::to_string((int)ms);
	}
};

#include "systemDisplay.h"

#include "systemPlayer.h"

#include "systemDamage.h"

#include "systemDead.h"

#include "systemAI.h"