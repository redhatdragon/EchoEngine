#include <DDECS.h>
#include <PhysicsEngineAABB.h>
#include <iostream>

class SystemDestructOnOverlap : public System {
	ComponentID destructOnOverlapComponentID = -1;
	ComponentID bodyComponentID = -1;
	ComponentID deadComponentID = -1;
public:
	virtual void init() {
		destructOnOverlapComponentID = ecs.registerComponent("destructOnOverlap", NULL);
		bodyComponentID = ecs.registerComponent("body", sizeof(BodyID));
		deadComponentID = ecs.registerComponent("dead", NULL);
	}
	virtual void run() {
		uint32_t deadCount = ecs.getComponentCount(destructOnOverlapComponentID);
		std::vector<EntityID> entities;
		for (uint32_t i = 0; i < deadCount; i++) {
			EntityID entity = ecs.getOwner(destructOnOverlapComponentID, i);
			BodyID bodyID = *(BodyID*)ecs.getEntityComponent(entity, bodyComponentID);
			auto& overlappingBodies = physics.getOverlappingBodies(bodyID);
			if(overlappingBodies.count)
				ecs.emplace(entity, deadComponentID, NULL);
		}
	}
	virtual const char* getName() {
		return "SystemDestructOnOverlap";
	}
};