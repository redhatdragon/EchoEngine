#include <DDECS.h>
#include <PhysicsEngineAABB.h>
#include <iostream>
#include <stdint.h>
#include "SystemUtilities.h"

class SystemUnitAI : public System {
	ComponentID bodyComponentID = -1;
	ComponentID moveToLocationComponentID = -1;
	ComponentID unitAIComponentID = -1;
public:
	virtual void init() {
		bodyComponentID = ecs.registerComponent("body", sizeof(BodyID));
		//moveToLocationComponentID = ecs.registerComponent("moveToLocation", sizeof(SystemUtilities::MoveToLocation));
		unitAIComponentID = ecs.registerComponent("unitAI", sizeof(SystemUtilities::UnitAI));
	}
	virtual void run() {
		//all units have a moveToLocation component >.>  Need something more dinstinctive.
		/*uint32_t unitAICount = ecs.getComponentCount(unitAIComponentID);
		SystemUtilities::UnitAI* unitAI = (SystemUtilities::UnitAI*)ecs.getComponentBuffer(unitAIComponentID);
		for (uint32_t i = 0; i < unitAICount; i++, unitAI++) {
			EntityID owner = ecs.getOwner(unitAIComponentID, i);
			if (ecs.entityHandleValid(unitAI->target, unitAI->targetHandle) == false)
				goto getNewTarget;
			//if enemy not in range anymore goto getNewTarget
			{
				constexpr uint32_t maxRange = 100;
				constexpr uint32_t maxRangeSqr = maxRange * maxRange;

			}
			goto moveToTarget;
			getNewTarget:

			moveToTarget:

			shootAtTarget:
			;
		}*/
	}
	virtual const char* getName() {
		return "SystemUnitAI";
	}
};