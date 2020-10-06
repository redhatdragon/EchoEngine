#include "systems.h"
#include "systemDamage.h"

void systemDamage() {
	if (detectOverlapComponentID == -1 || damageOnCollisionComponentID == -1 || healthComponentID == -1) return;
	auto damageOnCollisionCount = ecs.getComponentCount(damageOnCollisionComponentID);
	for (uint32_t i = 0; i < damageOnCollisionCount; i++) {
		EntityID damagingEntity = ecs.getOwner(damageOnCollisionComponentID, i);
		BodyID* damagingBody = (BodyID*)ecs.getEntityComponent(damagingEntity, bodyComponentID);
		auto overlappingBodies = physics.getOverlappingBodies(*damagingBody);
		for (uint32_t j = 0; j < overlappingBodies.count; j++) {
			auto otherBody = overlappingBodies[j];
			EntityID otherEntity = (EntityID)physics.getUserData(otherBody);
			if (ecs.getEntityComponent(otherEntity, detectOverlapComponentID) == nullptr) continue;
			uint32_t* health = (uint32_t*)ecs.getEntityComponent(otherEntity, healthComponentID);
			if (health == nullptr) continue;
			uint32_t* damage = (uint32_t*)ecs.getComponentBuffer(damageOnCollisionComponentID);
			damage = damage + i;
			if (*health < *damage) {
				*health = 0;
				ecs.emplace(otherEntity, deadComponentID, NULL);
				continue;
			}
			*health -= *damage;
		}
	}
}