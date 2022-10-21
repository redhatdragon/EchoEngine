#include <DDECS.h>
#include <PhysicsEngineAABB.h>
#include <iostream>
#include <stdint.h>
#include "SystemUtilities.h"

class SystemMoveTo : public System {
	ComponentID bodyComponentID = -1;
	ComponentID moveToLocationComponentID = -1;
public:
	virtual void init() {
		bodyComponentID = ecs.registerComponent("body", sizeof(BodyID));
		moveToLocationComponentID = ecs.registerComponent("moveToLocation", sizeof(SystemUtilities::MoveToLocation));
	}
	virtual void run() {
		uint32_t moveToLocationCount = ecs.getComponentCount(moveToLocationComponentID);
		SystemUtilities::MoveToLocation* moveBufferStart = (SystemUtilities::MoveToLocation*)ecs.getComponentBuffer(moveToLocationComponentID);
		SystemUtilities::MoveToLocation* moveBufferEnd = moveBufferStart+moveToLocationCount;
		uint32_t i = 0;
		for (SystemUtilities::MoveToLocation* move = moveBufferStart; move < moveBufferEnd; move++, i++) {
			EntityID owner = ecs.getOwner(moveToLocationComponentID, i);
			BodyID* bodyIDPtr = (BodyID*)ecs.getEntityComponent(owner, bodyComponentID);
			if (bodyIDPtr == nullptr) {
				std::cout << "Error: SystemMoveTo::run() can't move entity with moveToLocation component," << std::endl
					<< "'body' component required to obtain/alter position.  Skipping..." << std::endl;
				continue;
			}
			Vec2D<FixedPoint<>> vel;
			Vec2D<uint32_t> pos = physics.getPos<uint32_t>(*bodyIDPtr);
			vel.x = move->pos.x - pos.x;
			vel.y = move->pos.y - pos.y;
			vel.normalize();
			vel *= move->speed;
			physics.setVelocity(*bodyIDPtr, vel.x, vel.y);
		}
	}
	virtual const char* getName() {
		return "SystemMoveTo";
	}
};