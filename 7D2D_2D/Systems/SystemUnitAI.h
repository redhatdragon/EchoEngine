#include <DDECS.h>
#include <PhysicsEngineAABB.h>
#include <iostream>
#include <stdint.h>
#include "SystemUtilities.h"

class SystemUnitAI : public System {
	ComponentID bodyComponentID = -1;
	ComponentID unitAIComponentID = -1;
	ComponentID healthComponentID = -1;  //Used to get team
	ComponentID moveToLocationComponentID = -1;
	static constexpr uint32_t maxRange = 248;
	static constexpr uint32_t maxRangeSqr = maxRange * maxRange;
	static constexpr uint32_t maxShootingRange = 1240;
	static constexpr uint64_t maxShootingRangeSqr = maxShootingRange * maxShootingRange;
public:
	virtual void init() {
		bodyComponentID = ecs.registerComponent("body", sizeof(BodyID));
		unitAIComponentID = ecs.registerComponent("unitAI", sizeof(SystemUtilities::UnitAI));
		healthComponentID = ecs.registerComponent("health", sizeof(SystemUtilities::Health));
		moveToLocationComponentID = ecs.registerComponent("moveToLocation", sizeof(SystemUtilities::MoveToLocation));
	}
	virtual void run() {
		using namespace SystemUtilities;
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
				moveToWaypoint(unitAI, bodyIDPtr);
				continue;
			}
			auto pos = physics.getPos<int32_t>(*bodyIDPtr);
			unitAI->tickFireRate();
			Health* ownerHealth = (Health*)ecs.getEntityComponent(owner, healthComponentID);
			if (unitAI->canFire()) {
				shootIfInRange(unitAI, pos);
			}
			moveToWaypoint(unitAI, bodyIDPtr);
		}
	}
	void getNewTarget(SystemUtilities::UnitAI* unitAI, BodyID* bodyID, const std::vector<BodyID>& enemyBodyIDs) {
		if (enemyBodyIDs.size() == 0) return;
		BodyID targetBodyID = enemyBodyIDs[0];
		uint64_t targetDist = (physics.getPos<int32_t>(enemyBodyIDs[0]) - physics.getPos<int32_t>(*bodyID)).getDistanceSquared();
		for (uint32_t j = 1; j < enemyBodyIDs.size(); j++) {
			if ((physics.getPos<int32_t>(enemyBodyIDs[j]) - physics.getPos<int32_t>(*bodyID)).getDistanceSquared() < targetDist) {
				targetBodyID = enemyBodyIDs[j];
			}
		}
		unitAI->setTarget((EntityID)physics.getUserData(targetBodyID));
	}
	inline std::vector<BodyID> getEnemyBodyIDs(BodyID* bodyID) {
		std::vector<BodyID> IDs;
		{
			Vec2D<int32_t> pos = physics.getPos<int32_t>(*bodyID);
			Vec2D<int32_t> rangeVec = { maxRange, maxRange };
			pos -= rangeVec / 2;
			Vec2D<int32_t> limitVec = physics.getMaxPositions();
			int32_t limit = limitVec.x;
			if (limitVec.x < limitVec.y) 
				limit = limitVec.y;
			pos.bound(0, limit);
			rangeVec.bound(0, limit);
			//TODO: delegate this bounds checking to the below method call
			IDs = physics.getBodiesInRectRough(pos, rangeVec);
		}
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
			SystemUtilities::Health* otherHealth = (SystemUtilities::Health*)ecs.getEntityComponent(otherOwner, healthComponentID);
			if (otherHealth == nullptr) continue;
			if (otherHealth->team != health->team)
				enemyIDs.push_back(*elem);
		}
		return enemyIDs;
	}
	inline bool shootIfInRange(SystemUtilities::UnitAI* ownerAI, Vec2D<int32_t> ownerPos) {
		//EntityID otherEntity = *(EntityID*)physics.getUserData(otherBodyIDs[j]);
		EntityID otherEntity = ownerAI->target;
		BodyID* otherBody = (BodyID*)ecs.getEntityComponent(otherEntity, bodyComponentID);
		Vec2D<int32_t> otherPos = physics.getPos<int32_t>(*otherBody);
		uint64_t distanceSqr = ownerPos.getDistanceFromSquared(otherPos);
		if (distanceSqr <= maxShootingRangeSqr) {
			shoot(*ownerAI, ownerPos, otherPos);
			return true;
		}
		return false;
	}
	inline void shoot(SystemUtilities::UnitAI& ownerAI, 
		const Vec2D<int32_t>& ownerPos, const Vec2D<int32_t>& otherPos) {
		ownerAI.resetFireTimer();
		Vec2D<int32_t> directionAsInt = ownerPos - otherPos;
		Vec2D<FixedPoint<>> direction = { directionAsInt.x, directionAsInt.y };
		direction.normalize();
		Vec2D<FixedPoint<>> spawnPosFP = Vec2D<FixedPoint<>>{ ownerPos.x, ownerPos.y } - (direction * 20);
		EntityID bullet = SystemUtilities::spawnEntityAt("Entities/Bullet.txt", { (uint32_t)spawnPosFP.x.getAsInt(), (uint32_t)spawnPosFP.y.getAsInt() });
		SystemUtilities::MoveToLocation moveTo = { {otherPos.x, otherPos.y}, {1, 1}, 6 };
		ecs.emplace(bullet, moveToLocationComponentID, &moveTo);
	}
	inline void moveToWaypoint(SystemUtilities::UnitAI* unitAI, BodyID* bodyID) {
		//continue;
		Vec2D<uint32_t> ownerPos = physics.getPos<uint32_t>(*bodyID);
		Vec2D<uint32_t> moveToPos = unitAI->moveTo.pos;
		Vec2D<uint32_t> diff = moveToPos - ownerPos;
		//std::cout << (int32_t)diff.x << " - " << (int32_t)diff.y << std::endl;
		Vec2D<FixedPoint<>> vel = { diff.x, diff.y };
		//std::cout << vel.x.getAsFloat() << " v " << vel.y.getAsFloat() << std::endl;
		//vel *= 10;  //TODO: remove after testing
		vel.normalize();
		//std::cout << vel.x.getAsFloat() << " n " << vel.y.getAsFloat() << std::endl;
		vel *= 2;
		//std::cout << vel.x.getAsFloat() << " + " << vel.y.getAsFloat() << std::endl;
		physics.setVelocity(*bodyID, vel.x, vel.y);
	}
	inline void moveToTarget(SystemUtilities::UnitAI* unitAI, BodyID* bodyID) {
		std::cout << "moving to target" << std::endl;
		BodyID targetBodyID = *(BodyID*)ecs.getEntityComponent(unitAI->target, bodyComponentID);
		//unitAI->moveTo.pos = physics.getPos<uint32_t>(targetBodyID);
		Vec2D<uint32_t> ownerPos = physics.getPos<uint32_t>(*bodyID);
		Vec2D<uint32_t> targetPos = physics.getPos<uint32_t>(targetBodyID);
		Vec2D<uint32_t> diff = targetPos - ownerPos;
		Vec2D<FixedPoint<>> vel = { diff.x, diff.y };
		vel.normalize();
		vel *= 2;
		physics.setVelocity(targetBodyID, vel.x, vel.y);
	}
	virtual const char* getName() {
		return "SystemUnitAI";
	}
};