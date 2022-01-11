#include <DDECS.h>
#include <PhysicsEngineAABB.h>
#include "SystemUtilities.h"
#include <iostream>
#include <stdint.h>

class SystemDamage : public System {
	ComponentID healthComponentID = -1;
	ComponentID damageOnCollisionComponentID = -1;
	ComponentID bodyComponentID = -1;
	ComponentID deadComponentID = -1;
public:
	virtual void init() {
		healthComponentID = ecs.registerComponent("health", sizeof(SystemUtilities::Health));
		damageOnCollisionComponentID = ecs.registerComponent("damageOnCollision", sizeof(SystemUtilities::DamageOnCollision));
		bodyComponentID = ecs.registerComponent("body", sizeof(BodyID));
		deadComponentID = ecs.registerComponent("dead", NULL);
	}
	virtual void run() {
		SystemUtilities::DamageOnCollision* dmgBuffer = (SystemUtilities::DamageOnCollision*)ecs.getComponentBuffer(damageOnCollisionComponentID);
		SystemUtilities::DamageOnCollision* dmgOnCollisionEnd = dmgBuffer+ecs.getComponentCount(damageOnCollisionComponentID);
		uint32_t i = 0;
		for (SystemUtilities::DamageOnCollision* dmgOnCollision = dmgBuffer; dmgOnCollision < dmgOnCollisionEnd; dmgOnCollision++, i++) {
			EntityID damagingEntity = ecs.getOwner(damageOnCollisionComponentID, i);
			if (ecs.entityHasComponent(damagingEntity, bodyComponentID) == false) continue;
			BodyID damagingBody = ecs.getEntityComponentAs<BodyID>(damagingEntity, bodyComponentID);
			auto& overlappingBodies = physics.getOverlappingBodies(damagingBody);
			uint32_t overlappingCount = overlappingBodies.count;
			for (uint32_t j = 0; j < overlappingCount; j++) {
				EntityID otherEntity = (EntityID)physics.getUserData(overlappingBodies[j]);
				SystemUtilities::Health* otherHealth = (SystemUtilities::Health*)ecs.getEntityComponent(otherEntity, healthComponentID);
				if (otherHealth == nullptr) continue;
				FixedPoint<> totalAttack;
				totalAttack.setRaw(0);
				for (uint32_t k = 0; k < SystemUtilities::DAMAGE_TYPES::MAX; k++) {
					if (dmgOnCollision->modifiers[k].getRaw() == 0) continue;
					FixedPoint<> attack = dmgOnCollision->base * dmgOnCollision->modifiers[k];
					if (otherHealth->modifiers[k].getRaw() == 0) continue;
					totalAttack += attack / otherHealth->modifiers[k];
				}
				if (otherHealth->base <= totalAttack) {
					otherHealth->base.setRaw(0);
					ecs.emplace(otherEntity, deadComponentID, NULL);
					continue;
				}
				otherHealth->base -= totalAttack;
			}
		}
	}
	virtual const char* getName() {
		return "SystemDamage";
	}
};