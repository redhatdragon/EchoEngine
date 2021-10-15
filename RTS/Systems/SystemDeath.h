#include <DDECS.h>
#include <iostream>

class SystemDeath : public System {
	ComponentID deadComponentID = -1;
public:
	virtual void init() {
		deadComponentID = ecs.registerComponent("dead", NULL);
	}
	virtual void run() {
		clock_t startTime = clock();

		uint32_t deadCount = ecs.getComponentCount(deadComponentID);
		std::vector<EntityID> entities;
		for (uint32_t i = 0; i < deadCount; i++) {
			EntityID entity = ecs.getOwner(deadComponentID, i);
			entities.push_back(entity);
		}
		auto e = entities[65999];
		for (uint32_t i = 0; i < deadCount; i++) {
			auto e = entities[i];
			ecs.removeEntity(e);
		}

		ms = clock() - startTime;
	}
	virtual const char* getName() {
		return "SystemDeath";
	}
};