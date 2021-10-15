#include <DDECS.h>
#include <PhysicsEngineAABB.h>
#include <Asset.h>
#include <iostream>
#include <IO_API/IO_API.h>

class SystemDisplay : public System {
	ComponentID textureComponentID = -1;
	ComponentID bodyComponentID = -1;
	ComponentID cameraComponentID = -1;
public:
	virtual void init() {
		textureComponentID = ecs.registerComponent("texture", sizeof(TextureID));
		bodyComponentID = ecs.registerComponent("body", sizeof(BodyID));
		cameraComponentID = ecs.registerComponent("camera", NULL);
	}
	virtual void run() {
		clock_t startTime = clock();

		Vec2D<int32_t> camPos = { 0, 0 };
		if (ecs.getComponentCount(cameraComponentID)) {
			EntityID entity = ecs.getOwner(cameraComponentID, 0);
			if (ecs.entityHasComponent(entity, bodyComponentID)) {
				BodyID bodyID = ecs.getEntityComponentAs<BodyID>(entity, bodyComponentID);
				camPos = physics.getPos<int32_t>(bodyID);
			}
		}
		unsigned int winWidth, winHeight;
		getCanvasSize(&winWidth, &winHeight);
		int cx = -(camPos.x-winWidth / 2), cy = -(camPos.y-winHeight/2);

		uint32_t texCount = ecs.getComponentCount(textureComponentID);
		for (uint32_t i = 0; i < texCount; i++) {
			EntityID entity = ecs.getOwner(textureComponentID, i);
			if (ecs.entityHasComponent(entity, bodyComponentID) == false)
				continue;
			BodyID bodyID = ecs.getEntityComponentAs<BodyID>(entity, bodyComponentID);
			auto pos = physics.getPos<int32_t>(bodyID);
			auto siz = physics.getSize<int32_t>(bodyID);
			TextureID texID = ecs.getEntityComponentAs<TextureID>(entity, textureComponentID);
			drawTexture(TextureCodex::get(texID), pos.x+cx, pos.y+cy, siz.x, siz.y);
		}

		ms = clock() - startTime;
	}
	virtual const char* getName() {
		return "SystemDisplay";
	}
};