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
		uint32_t unitAICount = ecs.getComponentCount(unitAIComponentID);
		SystemUtilities::UnitAI* unitAI = (SystemUtilities::UnitAI*)ecs.getComponentBuffer(unitAIComponentID);
		for (uint32_t i = 0; i < unitAICount; i++, unitAI++) {
			EntityID owner = ecs.getOwner(unitAIComponentID, i);
			BodyID* bodyIDPtr = (BodyID*)ecs.getEntityComponent(owner, bodyComponentID);
			constexpr uint32_t maxRange = 248;
			constexpr uint32_t maxRangeSqr = maxRange * maxRange;
			constexpr uint32_t maxShootingRange = 124;
			constexpr uint32_t maxShootingRangeSqr = maxShootingRange * maxShootingRange;
			auto pos = physics.getPos<int32_t>(*bodyIDPtr);
			Vec2D<int32_t> rangeVec = { maxRange, maxRange };
			std::vector<BodyID> otherBodyIDs = physics.getBodiesInRectRough(pos, rangeVec);
			if (ecs.entityHandleValid(unitAI->target, unitAI->targetHandle) == false)
				goto getNewTarget;
			//if enemy not in range anymore goto getNewTarget
			for (uint32_t j = 0; j < otherBodyIDs.size(); j++) {
				EntityID otherEntity = *(EntityID*)physics.getUserData(otherBodyIDs[j]);
				if (unitAI->target == otherEntity) {
					auto otherPos = physics.getPos<int32_t>(otherBodyIDs[j]);
					uint64_t distanceSqr = pos.getDistanceFromSquared(otherPos);
					if (distanceSqr <= maxShootingRangeSqr)
						goto shootAtTarget;
					goto moveToTarget;
				}
			}
			//goto getNewTarget;
			getNewTarget:
			{
				if (otherBodyIDs.size() == 0) return;
				BodyID targetBodyID = otherBodyIDs[0];
				uint64_t targetDist = (physics.getPos<int32_t>(otherBodyIDs[0]) - physics.getPos<int32_t>(*bodyIDPtr)).getDistanceSquared();
				for (uint32_t j = 1; j < otherBodyIDs.size(); j++) {
					EntityID otherEntity = (EntityID)physics.getUserData(otherBodyIDs[j]);
					if ((physics.getPos<int32_t>(otherBodyIDs[j])-physics.getPos<int32_t>(*bodyIDPtr)).getDistanceSquared() < targetDist) {
						targetBodyID = otherBodyIDs[j];
					}
				}
				unitAI->target = (EntityID)physics.getUserData(targetBodyID);
			}
			return;
			moveToTarget:
			{
				BodyID targetBodyID = *(BodyID*)ecs.getEntityComponent(unitAI->target, bodyComponentID);
				//unitAI->moveTo.pos = physics.getPos<uint32_t>(targetBodyID);
				Vec2D<uint32_t> ownerPos = physics.getPos<uint32_t>(*bodyIDPtr);
				Vec2D<uint32_t> targetPos = physics.getPos<uint32_t>(targetBodyID);

				Vec2D<FixedPoint<>> vel;
				physics.setVelocity(targetBodyID, vel.x, vel.y);
			}
			return;
			shootAtTarget:
			
			return;
		}
	}
	virtual const char* getName() {
		return "SystemUnitAI";
	}
};