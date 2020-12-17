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

struct Path {
	FlatBuffer<Vec2D<int32_t>, 64> points;
	uint8_t currentIndex;
};

struct ConditionalRating {
	int32_t dist, rating;
};
struct AIValues {
	union {
		struct {
			int32_t chase, shoot, retreat;
		};
		int32_t values[3];
	};
	uint32_t getHighestValue() {
		uint32_t retValue = 0;
		for (uint32_t i = 0; i < 3; i++) {
			if (values[i] > values[retValue])
				retValue = i;
		}
		return retValue;
	}
};
struct AI {
	AIValues values;
	ConditionalRating conditionalRatings[3];
};

void registerComponents();

class System {
protected:
	clock_t ms = 0;  //must be updated in every run method
public:
	virtual void init() {};
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