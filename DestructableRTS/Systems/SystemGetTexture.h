#include <DDECS.h>
#include <PhysicsEngineAABB.h>
#include <Asset.h>
#include <Name.h>
#include <IO_API/IO_API.h>

class SystemGetTexture : public System {
	ComponentID textureComponentID = -1;
	ComponentID getTextureComponentID = -1;
public:
	virtual void init() {
		textureComponentID = ecs.registerComponent("texture", sizeof(TextureID));
		getTextureComponentID = ecs.registerComponent("getTexture", sizeof(Name));
	}
	virtual void run() {
		clock_t startTime = clock();

		uint32_t getTextureCount = ecs.getComponentCount(getTextureComponentID);
		Name* getTextureBuffer = (Name*)ecs.getComponentBuffer(getTextureComponentID);
		std::vector<EntityID> owningEntities;
		for (uint32_t i = 0; i < getTextureCount; i++) {
			Name texPath = getTextureBuffer[i];
			std::string totalPath = getDirData();
			//totalPath += "/";
			totalPath += texPath.c_str();
			TextureID tex = TextureCodex::add(totalPath);
			EntityID owner = ecs.getOwner(getTextureComponentID, i);
			ecs.emplace(owner, textureComponentID, &tex);
			owningEntities.push_back(owner);
		}
		for (auto entity : owningEntities)
			ecs.removeComponent(entity, getTextureComponentID);

		ms = clock() - startTime;
	}
	virtual const char* getName() {
		return "SystemGetTexture";
	}
};