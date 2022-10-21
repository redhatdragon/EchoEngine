#include <DDECS.h>
#include <PhysicsEngineAABB.h>
#include <Voxel/Voxel.h>
#include <iostream>

struct PlaceBlockComponent {
	VoxelBlockIndex blockIndex;
	VoxelChunkIndex chunkIndex;
	VoxelBlockType type;
};

class SystemVoxelPhysics : public System {
	ComponentID textureComponentID = -1;
	ComponentID bodyComponentID = -1;
public:
	virtual void init() {
		textureComponentID = ecs.registerComponent("texture", sizeof(TextureID));
		bodyComponentID = ecs.registerComponent("body", sizeof(BodyID));
		voxelWorld.init();
	}
	virtual void run() {
		voxelWorld.tick();
	}
	virtual const char* getName() {
		return "SystemVoxelPhysics";
	}
};