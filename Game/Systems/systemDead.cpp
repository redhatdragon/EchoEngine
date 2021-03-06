#include "systems.h"
#include "systemDead.h"

void SystemDead::run() {
	//auto deadComponentID = ecs.getComponentID("dead");
	//auto bodyComponentID = ecs.getComponentID("body");
	//auto textureComponentID = ecs.getComponentID("texture");
	//if (deadComponentID == -1) return;

	clock_t c = clock();

	struct {  //TODO: find a better solution without relying on lambda (C++11) features.
		void operator() (EntityID _entity) const {
			BodyID* bodyID = (BodyID*)ecs.getEntityComponent(_entity, bodyComponentID);
			auto pos = physics.getPos<float>(*bodyID);
			//std::cout << "Entity: " << _entity << " " << pos.x << " " << pos.y << std::endl;
			if (bodyID)
				physics.removeBody(*bodyID);
			TextureID* textureID = (TextureID*)ecs.getEntityComponent(_entity, textureComponentID);
			if (textureID) {

			}
			ecs.removeEntity(_entity);
		}
	} clean;

	std::vector<EntityID> entitiesToClean;

	auto deadCount = ecs.getComponentCount(deadComponentID);
	auto deadComp = ecs.getComponentBuffer(deadComponentID);
	for (uint32_t i = 0; i < deadCount; i++) {
		EntityID deadEntity = ecs.getOwner(deadComponentID, i);
		entitiesToClean.push_back(deadEntity);
	}
	auto suicideOnCollisionCount = ecs.getComponentCount(suicideOnCollisionComponentID);
	auto suicideOnCollisionComp = ecs.getComponentBuffer(suicideOnCollisionComponentID);
	for (uint32_t i = 0; i < suicideOnCollisionCount; i++) {
		EntityID entity = ecs.getOwner(suicideOnCollisionComponentID, i);
		BodyID* bodyID = (BodyID*)ecs.getEntityComponent(entity, bodyComponentID);
		if (bodyID == nullptr)
			continue;
		if (physics.getOverlappingBodies(*bodyID).count)
			entitiesToClean.push_back(entity);
	}

	for (auto entity : entitiesToClean) {
		clean(entity);
	}

	ms = clock() - c;
}