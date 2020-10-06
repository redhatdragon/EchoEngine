#include "systems.h"
#include "systemDisplay.h"

void systemDisplay() {
	clock_t c = clock();
	auto bodyComponentID = ecs.getComponentID("body");
	Vec2D<uint32_t> displayOffset = { 0, 0 };
	{  //Get camera offset
		int mx, my;
		unsigned int cx, cy;
		getMouseCanvasPos(&mx, &my);
		getCanvasSize(&cx, &cy);
		if (mx < 0 || mx > (int)cx || my < 0 || my > (int)cy)
			return;  //May be redundant.
		//ComponentID playerControllerComponentID = ecs.getComponentID("playerController");
		if (playerControllerComponentID != -1) {
			EntityID player1 = ecs.getOwner(playerControllerComponentID, 0);
			BodyID* body = (BodyID*)ecs.getEntityComponent(player1, bodyComponentID);
			if (body != NULL) {
				auto playerPos = physics.getPos<int32_t>(*body);
				displayOffset.x += playerPos.x - cx / 2; displayOffset.y += playerPos.y - cy / 2;
				displayOffset.x *= -1; displayOffset.y *= -1;
			}
		}
	}
	//auto textureComponentID = ecs.getComponentID("texture");
	BodyID* bodyIDs = (BodyID*)ecs.getComponentBuffer(bodyComponentID);
	auto bodyCount = ecs.getComponentCount(bodyComponentID);
	for (uint32_t i = 0; i < bodyCount; i++) {
		BodyID bodyID = bodyIDs[i];
		Vec2D<int32_t> pos = physics.getPos<int32_t>(bodyID);
		Vec2D<int32_t> siz = physics.getSize<int32_t>(bodyID);
		EntityID owner = ecs.getOwner(bodyComponentID, i);
		TextureID* texID = (TextureID*)ecs.getEntityComponent(owner, textureComponentID);
		if (texID == NULL)
			continue;
		auto tex = TextureCodex::get(*texID);
		drawTexture(tex, pos.x + displayOffset.x, pos.y + displayOffset.y, siz.x, siz.y);
	}
	const char* ms = "ms";
	std::string entityCount = "Entities: ";
	entityCount += std::to_string((int)ecs.getEntityCount());
	std::string physicsTime = "Physics: ";
	physicsTime += std::to_string((int)physics.getTime()); physicsTime += ms;
	std::string displayTime = "systemDisplay: ";
	displayTime += std::to_string((int)(clock() - c)); displayTime += ms;
	drawText(entityCount.c_str(), 16, 0, 12);
	drawText(physicsTime.c_str(), 16, 16, 12);
	drawText(displayTime.c_str(), 16, 32, 12);
	drawText(std::to_string(getFPS()).c_str(), 700, 0, 12);

	//if (getKeyState('f'))
		//throw;
}