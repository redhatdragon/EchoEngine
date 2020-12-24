#include "systems.h"
#include "systemAI.h"
#include "EntityObjectLoader.h"

ComponentID teamComponentID;
ComponentID getAIFromFileComponentID;
ComponentID AIComponentID;

enum CONDITION_TYPES {
	CHASE_IF_IN_RANGE,
	SHOOT_IF_IN_RANGE,
	RETREAT_IF_HEALTH_LESS_THAN
};

void SystemAI::init() {
	teamComponentID = ecs.registerComponent("team", sizeof(uint32_t));
	getAIFromFileComponentID = ecs.registerComponent("getAIFromFile", sizeof(Name));
	AIComponentID = ecs.registerComponent("AI", sizeof(AI));
}

bool _isComponentValidSize(ComponentObject& component) {
	if (component.size != sizeof(ConditionalRating)) {
		std::string errorStr = "";
		errorStr += component.name.c_str();
		errorStr += "'s component has invalid size, skipping AI initialization.";
		std::cout << "Runtime Warning: , %s" << errorStr << std::endl;
		return false;
	}
	return true;
}

void _initialize() {
	Name* bufferElement = (Name*)ecs.getComponentBuffer(getAIFromFileComponentID);
	uint32_t count = ecs.getComponentCount(getAIFromFileComponentID);
	std::vector<EntityID> entitiesToRemoveComponent;
	for (uint32_t i = 0; i < count; i++) {
		std::string fullPath = getDirData();
		fullPath += bufferElement->c_str();
		size_t fileSize = fileGetSize(fullPath.c_str());
		char* dataFromFile = (char*)malloc(fileSize);
		getFileText(fullPath.c_str(), dataFromFile);
		EntityObject entityObject = EntityObjectLoader::createEntityObjectFromString(dataFromFile);
		AI ai = { 0 };
		for (auto& component : entityObject.components) {
			if (_isComponentValidSize(component) == false) goto cleanAndAbort;
		}
		for (auto& component : entityObject.components) {
			if (component.name == "chaseEnemyIfInRange") {
				memcpy(&ai.conditionalRatings[CONDITION_TYPES::CHASE_IF_IN_RANGE], component.data, sizeof(ConditionalRating));
				continue;
			}
			if (component.name == "shootIfEnemyInRange") {
				memcpy(&ai.conditionalRatings[CONDITION_TYPES::SHOOT_IF_IN_RANGE], component.data, sizeof(ConditionalRating));
				continue;
			}
			if (component.name == "retreatIfHealthLessThan") {
				memcpy(&ai.conditionalRatings[CONDITION_TYPES::RETREAT_IF_HEALTH_LESS_THAN], component.data, sizeof(ConditionalRating));
				continue;
			}
		}

		{
			EntityID entity = ecs.getOwner(getAIFromFileComponentID, i);
			ecs.emplace(entity, AIComponentID, &ai);
			//ecs.removeComponent(entity, getAIFromFileComponentID);
			entitiesToRemoveComponent.push_back(entity);
		}
		bufferElement++;
		continue;

	cleanAndAbort:
		// TODO: We may need to swap this out for simply emplacing a dead component type into the entity if weird issues begin to arise.
		EntityID entity = ecs.getOwner(getAIFromFileComponentID, i);
		ecs.emplace(entity, deadComponentID, NULL);
		bufferElement++;
		continue;
	}
	for (auto e : entitiesToRemoveComponent) {
		ecs.removeComponent(e, getAIFromFileComponentID);
	}
}

inline std::vector<BodyID> _getOtherBodiesInRange(EntityID entity, std::vector<BodyID> bodiesToIgnore = std::vector<BodyID>()) {
	if (ecs.entityHasComponent(entity, teamComponentID) == false)
		return std::vector<BodyID>();

	BodyID owningBody = ecs.getEntityComponentAs<BodyID>(entity, bodyComponentID);
	bodiesToIgnore.push_back(owningBody);
	Vec2D<int32_t> ownerPos = physics.getPos<int32_t>(owningBody);
	std::vector<BodyID> bodiesInRange = physics.getBodiesInRectRough(ownerPos, Vec2D<int32_t>() = { 640, 640 });

	for (int32_t i = 0; i < bodiesInRange.size(); i++) {
		EntityID otherEntity = (EntityID)physics.getUserData(bodiesInRange[i]);
		if (ecs.entityHasComponent(otherEntity, teamComponentID) == false) goto removeElement;
		{
			if (ecs.entityHasComponent(otherEntity, teamComponentID) == false) goto removeElement;
			uint32_t otherTeam = ecs.getEntityComponentAs<uint32_t>(otherEntity, teamComponentID);
			uint32_t ownersTeam = ecs.getEntityComponentAs<uint32_t>(entity, teamComponentID);
			if (otherTeam == ownersTeam) goto removeElement;
			continue;
		}

		removeElement:
		bodiesToIgnore.push_back(bodiesInRange[i]);
	}

	for (auto bodyID : bodiesToIgnore) {
		for (size_t i = 0; i < bodiesInRange.size(); i++) {
			if (bodyID == bodiesInRange[i]) {
				bodiesInRange[i] = bodiesInRange[bodiesInRange.size() - 1];
				bodiesInRange.pop_back();
			}
		}
	}
	if (bodiesInRange.size() != 0)
		;// throw;
	return bodiesInRange;
}

void SystemAI::run() {
	clock_t c = clock();
	_initialize();

	AI* bufferElement = (AI*)ecs.getComponentBuffer(AIComponentID);
	uint32_t count = ecs.getComponentCount(AIComponentID);
	for (uint32_t i = 0; i < count; i++) {
		static constexpr int32_t chance_range = 60;
		int32_t chanceValue = rand() % chance_range;
		if (chanceValue != 0) continue;
		auto entity = ecs.getOwner(AIComponentID, i);
		BodyID bodyID = ecs.getEntityComponentAs<BodyID>(entity, bodyComponentID);
		Vec2D<int32_t> ownerPos = physics.getPos<int32_t>(bodyID);
		//uint32_t ownersTeam = -1;
		//if (ecs.entityHasComponent(entity, teamComponentID) == true)
			//ownersTeam = ecs.getEntityComponentAs<uint32_t>(entity, teamComponentID);
		std::vector<BodyID> bodiesInRange = _getOtherBodiesInRange(entity);
		BodyID closestEnemy;
		if (bodiesInRange.size() == 0) continue;
		closestEnemy = bodiesInRange[0];
		Vec2D<int32_t> enemyDist = physics.getPos<int32_t>(closestEnemy);
		float closestEnemyDist = enemyDist.getDistanceFrom(ownerPos);
		for (auto b : bodiesInRange) {
			Vec2D<int32_t> otherPos = physics.getPos<int32_t>(b);
			float dist = ownerPos.getDistanceFrom(otherPos);
			if (dist < closestEnemyDist) {
				closestEnemy = b;
				closestEnemyDist = dist;
			}
		}

		{
			ConditionalRating& conditionalRating = bufferElement->conditionalRatings[CONDITION_TYPES::CHASE_IF_IN_RANGE];
			float dist = conditionalRating.dist - closestEnemyDist;
			if (dist < 0) dist = 0;
			bufferElement->values.chase = dist * conditionalRating.rating;
		}
		{
			ConditionalRating& conditionalRating = bufferElement->conditionalRatings[CONDITION_TYPES::SHOOT_IF_IN_RANGE];
			float dist = conditionalRating.dist - closestEnemyDist;
			if (dist < 0) dist = 0;
			bufferElement->values.shoot = dist * conditionalRating.rating;
		}
		{
			ConditionalRating& conditionalRating = bufferElement->conditionalRatings[CONDITION_TYPES::RETREAT_IF_HEALTH_LESS_THAN];
			float dist = 0;
			if(ecs.entityHasComponent(entity, healthComponentID))
				dist = conditionalRating.dist - ecs.getEntityComponentAs<uint32_t>(entity, healthComponentID);
			if (dist < 0) dist = 0;
			bufferElement->values.retreat = dist * conditionalRating.rating;
		}

		//continue;
		switch (bufferElement->values.getHighestValue()) {
		case CONDITION_TYPES::CHASE_IF_IN_RANGE:
			std::cout << "CHASE" << std::endl;
			break;
		case CONDITION_TYPES::SHOOT_IF_IN_RANGE:
			std::cout << "SHOOT" << std::endl;
			break;
		case CONDITION_TYPES::RETREAT_IF_HEALTH_LESS_THAN:
			std::cout << "RETREAT" << std::endl;
			break;
		default:
			std::cout << bufferElement->values.getHighestValue() << std::endl;
			break;
		}

		bufferElement++;
	}
	ms = clock() - c;
}