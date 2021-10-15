#include "systems.h"

DDECS<24, 100000> ecs = DDECS<24, 100000>();
PhysicsEngine physics;

ComponentID bodyComponentID;
ComponentID playerControllerComponentID;
ComponentID textureComponentID;
ComponentID detectOverlapComponentID;
ComponentID damageOnCollisionComponentID;
ComponentID healthComponentID;
ComponentID deadComponentID;
ComponentID suicideOnCollisionComponentID;

void registerComponents() {
	bodyComponentID = ecs.registerComponent("body", sizeof(BodyID));
	playerControllerComponentID = ecs.registerComponent("playerController", 0);
	textureComponentID = ecs.registerComponent("texture", sizeof(TextureID));
	detectOverlapComponentID = ecs.registerComponent("detectOverlap", NULL);
	damageOnCollisionComponentID = ecs.registerComponent("damageOnCollision", sizeof(uint32_t));
	healthComponentID = ecs.registerComponent("health", sizeof(uint32_t));
	deadComponentID = ecs.registerComponent("dead", NULL);
	suicideOnCollisionComponentID = ecs.registerComponent("suicideOnCollision", NULL);
}