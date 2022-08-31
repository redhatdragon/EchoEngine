#include <DDECS.h>
#include <PhysicsEngineAABB.h>
#include <iostream>
#include <stdint.h>
#include <IO_API/IO_API.h>

class SystemController : public System {
	ComponentID controllerComponentID = -1;
	ComponentID bodyComponentID = -1;
public:
	virtual void init() {
		controllerComponentID = ecs.registerComponent("controller", NULL);
		bodyComponentID = ecs.registerComponent("body", sizeof(BodyID));
	}
	virtual void run() {
		if (ecs.getComponentCount(controllerComponentID) == 0) return;
		EntityID entity = ecs.getOwner(controllerComponentID, 0);
		BodyID* body = (BodyID*)ecs.getEntityComponent(entity, bodyComponentID);
		if (body == nullptr) return;
		float vx = 0, vy = 0;
		float speed = 3;
		if (getKeyState('w'))
			vy -= speed;
		if (getKeyState('s'))
			vy += speed;
		if (getKeyState('a'))
			vx -= speed;
		if (getKeyState('d'))
			vx += speed;
		physics.setVelocity(*body, vx, vy);
	}
	virtual const char* getName() {
		return "SystemController";
	}
};