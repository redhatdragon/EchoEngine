#include <DDECS.h>
#include <PhysicsEngineAABB.h>
#include <iostream>
#include <stdint.h>
#include "SystemUtilities.h"

class SystemUnitAI : public System {
	ComponentID bodyComponentID = -1;
	//ComponentID moveToLocationComponentID = -1;
	ComponentID unitAIComponentID = -1;
	ComponentID healthComponentID = -1;  //Used to get team
	static constexpr uint32_t maxRange = 248;
	static constexpr uint32_t maxRangeSqr = maxRange * maxRange;
	static constexpr uint32_t maxShootingRange = 124;
	static constexpr uint32_t maxShootingRangeSqr = maxShootingRange * maxShootingRange;
public:
	virtual void init() {
		bodyComponentID = ecs.registerComponent("body", sizeof(BodyID));
		//moveToLocationComponentID = ecs.registerComponent("moveToLocation", sizeof(SystemUtilities::MoveToLocation));
		unitAIComponentID = ecs.registerComponent("unitAI", sizeof(SystemUtilities::UnitAI));
		healthComponentID = ecs.registerComponent("health", sizeof(SystemUtilities::Health));
	}
	virtual void run() {
		//all units have a moveToLocation component >.>  Need something more dinstinctive.
		uint32_t unitAICount = ecs.getComponentCount(unitAIComponentID);
		SystemUtilities::UnitAI* unitAI = (SystemUtilities::UnitAI*)ecs.getComponentBuffer(unitAIComponentID);
		for (uint32_t i = 0; i < unitAICount; i++, unitAI++) {
			EntityID owner = ecs.getOwner(unitAIComponentID, i);
			BodyID* bodyIDPtr = (BodyID*)ecs.getEntityComponent(owner, bodyComponentID);
			std::vector<BodyID> enemyBodyIDs = getEnemyBodyIDs(bodyIDPtr);
			if(unitAI->hasValidTarget() == false)
				getNewTarget(unitAI, bodyIDPtr, enemyBodyIDs);
			if (unitAI->hasValidTarget() == false) {
				//printf("Not getting new target\n");
				moveToWaypoint(unitAI, bodyIDPtr);
				continue;
			}
			//printf("Found Target\n");
			auto pos = physics.getPos<int32_t>(*bodyIDPtr);
			shootIfInRange(unitAI, pos);
			moveToWaypoint(unitAI, bodyIDPtr);
			//moveToTarget(unitAI, bodyIDPtr);
		}
	}
	void getNewTarget(SystemUtilities::UnitAI* unitAI, BodyID* bodyID, const std::vector<BodyID>& enemyBodyIDs) {
		if (enemyBodyIDs.size() == 0) return;
		BodyID targetBodyID = enemyBodyIDs[0];
		uint64_t targetDist = (physics.getPos<int32_t>(enemyBodyIDs[0]) - physics.getPos<int32_t>(*bodyID)).getDistanceSquared();
		for (uint32_t j = 1; j < enemyBodyIDs.size(); j++) {
			EntityID otherEntity = (EntityID)physics.getUserData(enemyBodyIDs[j]);
			if ((physics.getPos<int32_t>(enemyBodyIDs[j]) - physics.getPos<int32_t>(*bodyID)).getDistanceSquared() < targetDist) {
				targetBodyID = enemyBodyIDs[j];
			}
		}
		unitAI->setTarget((EntityID)physics.getUserData(targetBodyID));
	}
	inline std::vector<BodyID> getEnemyBodyIDs(BodyID* bodyID) {
		auto pos = physics.getPos<int32_t>(*bodyID);
		Vec2D<int32_t> rangeVec = { maxRange, maxRange };
		std::vector<BodyID> IDs = physics.getBodiesInRectRough(pos, rangeVec);
		std::vector<BodyID> enemyIDs;
		enemyIDs.reserve(IDs.size());
		EntityID owner = (EntityID)physics.getUserData(*bodyID);
		SystemUtilities::Health* health = (SystemUtilities::Health*)ecs.getEntityComponent(owner, healthComponentID);
		if (health == nullptr) {
			std::cout << "SystemUnitAI::getEnemyBodyIDs()'s bodyID's owner has no health component" << std::endl;
			return enemyIDs;
		}
		for (BodyID* elem = &IDs[0], *end = &IDs[IDs.size()-1]; elem != end; elem++) {
			EntityID otherOwner = (EntityID)physics.getUserData(*elem);
			SystemUtilities::Health * otherHealth = (SystemUtilities::Health*)ecs.getEntityComponent(otherOwner, healthComponentID);
			if (otherHealth == nullptr) continue;
			if (otherHealth->team != health->team)
				enemyIDs.push_back(*elem);
		}
		return enemyIDs;
	}
	inline void shootIfInRange(SystemUtilities::UnitAI* unitAI, Vec2D<int32_t> ownerPos) {
		//EntityID otherEntity = *(EntityID*)physics.getUserData(otherBodyIDs[j]);
		EntityID otherEntity = unitAI->target;
		BodyID* otherBody = (BodyID*)ecs.getEntityComponent(otherEntity, bodyComponentID);
		Vec2D<int32_t> otherPos = physics.getPos<int32_t>(*otherBody);
		uint64_t distanceSqr = ownerPos.getDistanceFromSquared(otherPos);
		if (distanceSqr <= maxShootingRangeSqr) {
			Vec2D<int32_t> direction = ownerPos-otherPos;
			direction.normalize();
			Vec2D<int32_t> spawnPos = ownerPos + (direction * 40);
			//SystemUtilities::spawnEntityAt("Entities/Bullet.txt", { (uint32_t)spawnPos.x, (uint32_t)spawnPos.y} );
			//SystemUtilities::spawnEntityAt("Entities/Bullet.txt", { (uint32_t)400, (uint32_t)20 });
		}
	}
	inline void moveToWaypoint(SystemUtilities::UnitAI* unitAI, BodyID* bodyID) {
		//continue;
		Vec2D<uint32_t> ownerPos = physics.getPos<uint32_t>(*bodyID);
		Vec2D<uint32_t> moveToPos = unitAI->moveTo.pos;
		Vec2D<uint32_t> diff = moveToPos - ownerPos;
		//std::cout << (int32_t)diff.x << " - " << (int32_t)diff.y << std::endl;
		Vec2D<FixedPoint<>> vel = { diff.x, diff.y };
		//std::cout << vel.x.getAsFloat() << " v " << vel.y.getAsFloat() << std::endl;
		vel *= 10;  //TODO: remove after testing
		vel.normalize();
		//std::cout << vel.x.getAsFloat() << " n " << vel.y.getAsFloat() << std::endl;
		vel *= 2;
		//std::cout << vel.x.getAsFloat() << " + " << vel.y.getAsFloat() << std::endl;
		physics.setVelocity(*bodyID, vel.x, vel.y);
	}
	inline void moveToTarget(SystemUtilities::UnitAI* unitAI, BodyID* bodyID) {
		std::cout << "hit" << std::endl;
		BodyID targetBodyID = *(BodyID*)ecs.getEntityComponent(unitAI->target, bodyComponentID);
		//unitAI->moveTo.pos = physics.getPos<uint32_t>(targetBodyID);
		Vec2D<uint32_t> ownerPos = physics.getPos<uint32_t>(*bodyID);
		Vec2D<uint32_t> targetPos = physics.getPos<uint32_t>(targetBodyID);
		Vec2D<uint32_t> diff = targetPos - ownerPos;
		Vec2D<FixedPoint<>> vel = { diff.x, diff.y };
		vel.normalize();
		vel *= 6;
		physics.setVelocity(targetBodyID, vel.x, vel.y);
	}
	virtual const char* getName() {
		return "SystemUnitAI";
	}
};