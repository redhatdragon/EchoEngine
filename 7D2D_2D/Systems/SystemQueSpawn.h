#include <DDECS.h>
#include <PhysicsEngineAABB.h>
#include <iostream>
#include <stdint.h>
#include <DArray.h>
#include <FlatBuffer.h>
#include <Name.h>
#include "SystemUtilities.h"

class SystemQueSpawn : public System {
	ComponentID bodyComponentID = -1;
	ComponentID queComponentID = -1;
	ComponentID queSpawnComponentID = -1;
	ComponentID queTickComponentID = -1;
	ComponentID quableEntitiesComponentID = -1;
	//ComponentID moveToLocationComponentID = -1;
	ComponentID unitAIComponentID = -1;
	ComponentID queWaypointComponentID = -1;
	ComponentID healthComponentID = -1;  //to set team
public:
	virtual void init() {
		bodyComponentID = ecs.registerComponent("body", sizeof(BodyID));
		queComponentID = ecs.registerComponent("que", sizeof(FlatBuffer<Name, 5>));
		queSpawnComponentID = ecs.registerComponent("queSpawn", sizeof(Vec2D<uint32_t>));
		queTickComponentID = ecs.registerComponent("queTick", sizeof(uint32_t));
		quableEntitiesComponentID = ecs.registerComponent("quableEntities", sizeof(DArray<Name>));
		//moveToLocationComponentID = ecs.registerComponent("moveToLocation", sizeof(SystemUtilities::MoveToLocation));
		unitAIComponentID = ecs.registerComponent("unitAI", sizeof(SystemUtilities::UnitAI));
		queWaypointComponentID = ecs.registerComponent("queWaypoint", sizeof(uint32_t) * 2);
		healthComponentID = ecs.registerComponent("health", sizeof(SystemUtilities::Health));
	}
	virtual void run() {
		Vec2D<uint32_t>* queSpawns = (Vec2D<uint32_t>*)ecs.getComponentBuffer(queSpawnComponentID);
		uint32_t queSpawnCount = ecs.getComponentCount(queSpawnComponentID);
		for (uint32_t i = 0; i < queSpawnCount; i++) {
			Vec2D<uint32_t>* queSpawn = &queSpawns[i];
			EntityID owner = ecs.getOwner(queSpawnComponentID, i);
			BodyID* bodyIDPtr = (BodyID*)ecs.getEntityComponent(owner, bodyComponentID);
			if (bodyIDPtr == nullptr) {
				std::cout << "Error: SystemQueSpawn{} factory like entity without bodyID component" << std::endl;
				continue;
			}
			FlatBuffer<Name, 5>& que = *(FlatBuffer<Name, 5>*)ecs.getEntityComponent(owner, queComponentID);
			uint32_t& queTick = *(uint32_t*)ecs.getEntityComponent(owner, queTickComponentID);
			if (&que == nullptr || que.count == 0 || &queTick == nullptr)
				continue;
			if (queTick > 0) {
				queTick--;
				continue;
			}
			queTick = 300;
			DArray<Name>& quableEntities = *(DArray<Name>*)ecs.getEntityComponent(owner, quableEntitiesComponentID);
			if (&quableEntities == nullptr) {
				std::cout << "Error: SystemQueSpawn{} factory like entity without quableEntities component" << std::endl;
				continue;
			}
			if (que.count > 1) {
				std::vector<Name> ques;
				for (uint32_t j = 1; j < que.count; j++) {
					ques.push_back(que[j]);
				}
				memcpy(&que[0], &ques[0], sizeof(Name)*ques.size());  //TODO: what?
			}
			que.pop();
			std::string entityPath = getDirData();
			entityPath += "Entities/Solder.txt";
			EntityID spawnedEntity = SystemUtilities::spawnEntityAt(entityPath, {queSpawn->x, queSpawn->y});
			//printf("spawn\n");
			//std::cout << "Pos is: " << queSpawn->x << ", " << queSpawn->y << std::endl;
			uint32_t* queWaypoint = (uint32_t*)ecs.getEntityComponent(owner, queWaypointComponentID);
			if (queWaypoint == nullptr) continue;
			SystemUtilities::MoveToLocation moveTo = { {queWaypoint[0], queWaypoint[1]}, {16, 16}, 5 };
			SystemUtilities::UnitAI unitAI;
			unitAI.moveTo = moveTo;
			ecs.emplace(spawnedEntity, unitAIComponentID, &unitAI);
			//std::cout << spawnedEntity << " " << queWaypoint[0] << " " << queWaypoint[1] << " " 
				//<< queSpawn->x << " " << queSpawn->y << std::endl;
			SystemUtilities::Health* ownerHealth = (SystemUtilities::Health*)ecs.getEntityComponent(owner, healthComponentID);
			SystemUtilities::Health* spawnedEntityHealth = (SystemUtilities::Health*)ecs.getEntityComponent(spawnedEntity, healthComponentID);
			if (spawnedEntityHealth == nullptr) {
				printf("Error: SystemQueSpawn::run(), spawnedEntityHealth == nullptr\n");
				throw;
			}
			spawnedEntityHealth->team = ownerHealth->team;
		}
	}
	virtual const char* getName() {
		return "SystemQueSpawn";
	}
};