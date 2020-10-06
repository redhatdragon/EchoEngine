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

void registerComponents();

#include "systemDisplay.h"

#include "systemPlayer.h"

#include "systemDamage.h"

#include "systemDead.h"