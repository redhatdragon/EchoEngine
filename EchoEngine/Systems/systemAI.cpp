#include "systems.h"
#include "systemAI.h"
#include "EntityObjectLoader.h"
#include <IO_API/IO_API.h>

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

std::unordered_map<std::string, std::string> filesDataMap;

void _t_pre_init(void* data) {
	struct T_Data {
		uint32_t startPos;
		uint32_t endPos;
		std::vector<EntityID>& entitiesToRemoveComponent;
	};
	T_Data* tdata = (T_Data*)data;
	uint32_t endPos = tdata->endPos;
	Name* bufferElement = (Name*)ecs.getComponentBuffer(getAIFromFileComponentID);
	for (uint32_t i = tdata->startPos; i < endPos; i++) {
		std::string fullPath = getDirData();
		fullPath += bufferElement->c_str();
		if (filesDataMap.find(fullPath) == filesDataMap.end()) continue;
		EntityObject entityObject = EntityObjectLoader::createEntityObjectFromString(filesDataMap[fullPath]);
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
			tdata->entitiesToRemoveComponent.push_back(entity);
		}
		bufferElement++;
		continue;

	cleanAndAbort:
		EntityID entity = ecs.getOwner(getAIFromFileComponentID, i);
		ecs.emplace(entity, deadComponentID, NULL);
		bufferElement++;
		continue;
	}
}

void _initialize() {
	Name* bufferElement = (Name*)ecs.getComponentBuffer(getAIFromFileComponentID);
	uint32_t count = ecs.getComponentCount(getAIFromFileComponentID);
	std::vector<EntityID> entitiesToRemoveComponent;

	for (uint32_t i = 0; i < count; i++) {
		std::string fullPath = getDirData();
		fullPath += bufferElement->c_str();
		if (filesDataMap.find(fullPath) == filesDataMap.end()) continue;
		size_t fileSize = fileGetSize(fullPath.c_str());
		if (fileSize == 0) continue;
		char* dataFromFile = (char*)malloc(fileSize);
		if (dataFromFile == NULL) continue;
		bool success = getFileText(fullPath.c_str(), dataFromFile);
		if (success == false) continue;
		filesDataMap[fullPath] = dataFromFile;
	}

	struct T_Data {
		uint32_t startPos;
		uint32_t endPos;
		std::vector<EntityID>& entitiesToRemoveComponent;
	};
	std::vector<T_Data> tdatas;

	goto singleThreaded;  //TODO: fix multithreading or remove it (seems to not have a performance impact)
	uint32_t threadCount = ThreadPoolGetFreeThreadCount(threadPool);
	uint32_t chunkPerThread = count / threadCount;
	if (chunkPerThread < 1000) goto singleThreaded;
	for (uint32_t i = 0; i < threadCount; i++) {
		T_Data tdata = { i * chunkPerThread, i * chunkPerThread + chunkPerThread, entitiesToRemoveComponent };
		tdatas.push_back(tdata);
		ThreadPoolGiveTask(threadPool, _t_pre_init, &tdatas.end());
		/*for (auto e : tdata.entitiesToRemoveComponent) {
			for (auto oe : entitiesToRemoveComponent) {
				if (oe == e) goto skip;
			}
			entitiesToRemoveComponent.push_back(e);
			skip:
		}*/

	}
	{
		uint32_t leftover = count % threadCount;
		T_Data tdata = { count - leftover, count, entitiesToRemoveComponent };
		tdatas.push_back(tdata);
		ThreadPoolGiveTask(threadPool, _t_pre_init, &tdatas.end());
	}
	goto finish;

singleThreaded:
	for (uint32_t i = 0; i < count; i++) {
		std::string fullPath = getDirData();
		fullPath += bufferElement->c_str();
		if (filesDataMap.find(fullPath) == filesDataMap.end()) continue;
		EntityObject entityObject = EntityObjectLoader::createEntityObjectFromString(filesDataMap[fullPath]);
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
		EntityID entity = ecs.getOwner(getAIFromFileComponentID, i);
		ecs.emplace(entity, deadComponentID, NULL);
		bufferElement++;
		continue;
	}
finish:
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
			if (ecs.entityHasComponent(entity, healthComponentID))
				dist = conditionalRating.dist - ecs.getEntityComponentAs<uint32_t>(entity, healthComponentID);
			if (dist < 0) dist = 0;
			bufferElement->values.retreat = dist * conditionalRating.rating;
		}

		//continue;
		switch (bufferElement->values.getHighestValue()) {
		case CONDITION_TYPES::CHASE_IF_IN_RANGE:
			//std::cout << "CHASE" << std::endl;
			break;
		case CONDITION_TYPES::SHOOT_IF_IN_RANGE:
			//std::cout << "SHOOT" << std::endl;
			break;
		case CONDITION_TYPES::RETREAT_IF_HEALTH_LESS_THAN:
			//std::cout << "RETREAT" << std::endl;
			break;
		default:
			std::cout << bufferElement->values.getHighestValue() << std::endl;
			break;
		}

		bufferElement++;
	}
	ms = clock() - c;
}