#include "systems.h"
#include "systemPlayer.h"

void systemPlayer() {
	static uint64_t shootingDelay = 100;
	auto playerControllerCount = ecs.getComponentCount(playerControllerComponentID);
	for (uint32_t i = 0; i < playerControllerCount; i++) {
		EntityID player = ecs.getOwner(playerControllerComponentID, i);
		auto bodyID = ecs.getComponentID("body");
		BodyID* body = (BodyID*)ecs.getEntityComponent(player, bodyID);
		if (body == nullptr) continue;

		float vx = 0, vy = 0;
		constexpr float speed = 3;
		if (getKeyState('w')) vy -= speed;
		if (getKeyState('s')) vy += speed;
		if (getKeyState('a')) vx -= speed;
		if (getKeyState('d')) vx += speed;
		physics.setVelocity(*body, vx, vy);

		uint8_t leftButton;
		getMouseState(&leftButton, NULL, NULL);
		if (leftButton) {
			//printf("Hit ");
			if (shootingDelay < 3) break;
			int mx, my;
			unsigned int cx, cy;
			getMouseCanvasPos(&mx, &my);
			getCanvasSize(&cx, &cy);
			if (mx < 0 || mx > cx || my < 0 || my > cy)
				return;  //May be redundant.
			Vec2D<float> spawnPos = { mx, my };
			auto playerPos = physics.getPos<float>(*body);
			auto playerSiz = physics.getSize<float>(*body);
			playerSiz /= 2;
			spawnPos.x -= cx / 2 - playerPos.x; spawnPos.y -= cy / 2 - playerPos.y;
			playerPos += playerSiz;
			auto direction = spawnPos;
			direction -= playerPos;
			direction.normalize();
			spawnPos = direction;
			spawnPos *= 124;
			spawnPos += playerPos;
			auto velocity = direction * 17;

			EntityID bullet = ecs.getNewEntity();
			TextureID textureID = TextureCodex::add("data/textures/Bullet.png");
			ecs.emplace(bullet, textureComponentID, &textureID);
			BodyID bodyID = physics.addBodyRect(spawnPos.x, spawnPos.y, 8, 8);
			physics.setVelocity(bodyID, velocity.x, velocity.y);
			physics.setUserData(bodyID, (void*)bullet);
			ecs.emplace(bullet, bodyComponentID, &bodyID);
			ecs.emplace(bullet, suicideOnCollisionComponentID, NULL);
			uint32_t damage = 25;
			ecs.emplace(bullet, damageOnCollisionComponentID, &damage);
			shootingDelay = 0;
			break;
		}
	}
	shootingDelay++;
}