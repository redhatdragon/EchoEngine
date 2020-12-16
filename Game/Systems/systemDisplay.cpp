#include "systems.h"
#include "systemDisplay.h"

void SystemDisplay::run() {
	clock_t c = clock();
	auto bodyComponentID = ecs.getComponentID("body");
	Vec2D<uint32_t> displayOffset = { 0, 0 };
	unsigned int canvasWidth, canvasHeight;
	getCanvasSize(&canvasWidth, &canvasHeight);
	{  //Get camera offset
		int mx, my;
		getMouseCanvasPos(&mx, &my);
		if (mx < 0 || mx > (int)canvasWidth || my < 0 || my > (int)canvasHeight)
			return;  //May be redundant.
		//ComponentID playerControllerComponentID = ecs.getComponentID("playerController");
		if (playerControllerComponentID != -1) {
			EntityID player1 = ecs.getOwner(playerControllerComponentID, 0);
			BodyID* body = (BodyID*)ecs.getEntityComponent(player1, bodyComponentID);
			if (body != NULL) {
				auto playerPos = physics.getPos<int32_t>(*body);
				displayOffset.x += playerPos.x - canvasWidth / 2; displayOffset.y += playerPos.y - canvasHeight / 2;
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
		//if(pos.x+siz.x < displayOffset.x || pos.x > displayOffset.x + canvasWidth
		//|| pos.y+siz.y < displayOffset.y || pos.y > displayOffset.y + canvasHeight)
			//continue;
		drawTexture(tex, pos.x + displayOffset.x, pos.y + displayOffset.y, siz.x, siz.y);
	}
	
	ms = clock() - c;
}