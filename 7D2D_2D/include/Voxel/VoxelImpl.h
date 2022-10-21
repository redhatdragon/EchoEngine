#pragma once
#include "Voxel.h"

void VoxelWorld::tick() {
	tickCheckStability();
}

void VoxelWorld::placeBlock(const VoxelBlock& block, const Vec2D<uint32_t>& pos) {
	BlockPos blockPos = getBlockPosFromVecPos(pos);
	placeBlock(block, blockPos);
}
void VoxelWorld::placeBlock(const VoxelBlock& block, BlockPos blockPos) {
	//If 3D we'd do a chunks[chunkIndex].loadIfUnloaded(); before accessing the block data
	VoxelBlock& blockToModify = chunks[blockPos.chunkIndex].blocks[blockPos.blockIndex];
	blockToModify = block;
	deferStabilityCheck(blockPos);
}
void VoxelWorld::placeBlock(VoxelBlockType typeID, const Vec2D<uint32_t>& pos) {
	BlockPos blockPos = getBlockPosFromVecPos(pos);
	placeBlock(typeID, blockPos);
}
void VoxelWorld::placeBlock(VoxelBlockType typeID, BlockPos blockPos) {
	VoxelBlock block;
	block.init(typeID);
	placeBlock(block, blockPos);
}

void VoxelWorld::removeBlock(const Vec2D<uint32_t>& pos) {
	BlockPos blockPos = getBlockPosFromVecPos(pos);
	removeBlock(blockPos);
}
void VoxelWorld::removeBlock(BlockPos blockPos) {
	//placeBlock(AIR_TYPE_ID, chunkIndex, blockIndex);  //this would accomplish the task to but...
	VoxelBlock& blockToModify = chunks[blockPos.chunkIndex].blocks[blockPos.blockIndex];
	blockToModify.typeID = AIR_TYPE_ID;
	FlatBuffer<BlockPos, 4> neighborPositions = getValidNeighborPositions(blockPos);
	uint32_t neighborCount = neighborPositions.count;
	for (uint32_t i = 0; i < neighborCount; i++) {
		deferStabilityCheck(neighborPositions[i]);
	}
}



BlockPos VoxelWorld::getBlockPosFromVecPos(const Vec2D<uint32_t>& pos) {
	BlockPos retValue;
	retValue.chunkIndex = pos.x / chunks->MAX_WIDTH;
	retValue.blockIndex = (pos.x % chunks->MAX_WIDTH) + (pos.y * chunks->MAX_WIDTH);
	return retValue;
}
Vec2D<uint32_t> VoxelWorld::getVecPosFromBlockPos(BlockPos blockPos) {
	Vec2D<uint32_t> retValue;
	retValue.x = (blockPos.chunkIndex * chunks->MAX_WIDTH) + 
		(blockPos.blockIndex % chunks->MAX_WIDTH);
	retValue.y = chunks->MAX_HEIGHT - (blockPos.blockIndex / chunks->MAX_WIDTH);
	return retValue;
}
void VoxelWorld::deferStabilityCheck(BlockPos blockPos) {
	if (stabilityQue.has(blockPos) == false)  //TODO: make this multithreaded if past some threshold
		stabilityQue.push(blockPos);
}
inline FlatBuffer<BlockPos, 4> VoxelWorld::getValidNeighborPositions(BlockPos blockPos) {
	struct IsBlockPosValid {
		static bool check(const Vec2D<uint32_t>& pos) {
			return true;
		}
	};

	FlatBuffer<BlockPos, 4> retValue;

	Vec2D<uint32_t> blockVecPos = getVecPosFromBlockPos(blockPos);
	//TODO: decide if we should exclude bedrock and other things potentially...
	if (blockVecPos.y > 1) {  //ignores bedrock...
		blockVecPos.y--;
		if (IsBlockPosValid::check(blockVecPos))
			retValue.push(getBlockPosFromVecPos(blockVecPos));
		blockVecPos.y++;
	}
	if (blockVecPos.y < 255) {
		blockVecPos.y++;
		if (IsBlockPosValid::check(blockVecPos))
			retValue.push(getBlockPosFromVecPos(blockVecPos));
		blockVecPos.y--;
	}
	if (blockVecPos.x > 0) {
		blockVecPos.x--;
		if (IsBlockPosValid::check(blockVecPos))
			retValue.push(getBlockPosFromVecPos(blockVecPos));
		blockVecPos.x++;
	}
	if (blockVecPos.x < getWorldTotalWidth()) {	
		blockVecPos.x++;
		if (IsBlockPosValid::check(blockVecPos))
			retValue.push(getBlockPosFromVecPos(blockVecPos));
		blockVecPos.x--;
	}
	return retValue;
}

VoxelBlock& VoxelWorld::getBlock(const BlockPos& blockPos) {
	return chunks[blockPos.chunkIndex].blocks[blockPos.blockIndex];
}

void VoxelWorld::tickCheckStability() {

	uint32_t count = stabilityQue.count;
	for (uint32_t i = 0; i < count; i++) {
		BlockPos& blockPos = stabilityQue[i];
		auto validNeighborBlocks = getValidNeighborPositions(blockPos);
		// recalculate stability
		VoxelBlock block = getBlock(blockPos);
		uint32_t totalStability = 0;
		uint32_t neighborCount = validNeighborBlocks.count;
		for (uint32_t j = 0; j < neighborCount; j++) {
			VoxelBlock neighborBlock = getBlock(validNeighborBlocks[j]);
			if (neighborBlock.isAir())
				break;

			recalculateStabilityInArea(validNeighborBlocks[j]);
		}
	}
}

void VoxelWorld::recalculateStabilityInArea(BlockPos blockPos) {
	//WARNING: we assume that these checks never go off near the borders of the world
	Vec2D<uint32_t> blockVecPos = getVecPosFromBlockPos(blockPos);
	stabilityBackBuffer.init(blockVecPos.x - updateWidth / 2);
	Vec2D<uint32_t> currentBlockVecPos = blockVecPos;
	for (uint32_t y = 2; y < 255; y++) {
		currentBlockVecPos.y = y;
		uint32_t startX = blockVecPos.x - updateWidth / 2;
		uint32_t endX = startX + updateWidth;
		for (uint32_t x = startX; x < endX; x++) {
			currentBlockVecPos.x = x;
			BlockPos currentBlockPos = getBlockPosFromVecPos(currentBlockVecPos);
			VoxelBlock currentBlock = getBlock(currentBlockPos);
			if (currentBlock.isSupporting()) {
				startRecalculateStabilityFromAnchoredBlock(currentBlockPos);
			}
		}
	}
	stabilityBackBuffer.applyValues();
}

void VoxelWorld::startRecalculateStabilityFromAnchoredBlock(BlockPos blockPos) {
	Vec2D<uint32_t> blockVecPos = getVecPosFromBlockPos(blockPos);
	FlatBuffer<BlockPos, 4> neighborPositions = getValidNeighborPositions(blockPos);
	uint32_t neighborCount = neighborPositions.count;
	for (uint32_t i = 0; i < neighborCount; i++) {
		const BlockPos& neighborPos = neighborPositions[i];
		Vec2D<uint32_t> neighborVecPos = getVecPosFromBlockPos(neighborPos);
		//must be on same elevation to know it's not another anchored block
		if (blockVecPos.y != neighborVecPos.y)
			continue;
		VoxelBlock& neighborBlock = getBlock(neighborPos);
		auto maxLoad = neighborBlock.getVoxelBlockMetaData().maxLoad;
		stabilityBackBuffer.add(neighborVecPos, maxLoad);
		recursiveRecalculateStabilityForBlock(neighborPos, blockPos);
	}
}

void VoxelWorld::recursiveRecalculateStabilityForBlock(BlockPos blockPos, BlockPos parentBlockPos) {
	FlatBuffer<BlockPos, 4> neighborPositions = getValidNeighborPositions(blockPos);
	uint32_t neighborCount = neighborPositions.count;
	if (neighborCount == 1)  //No neighbors except parent, so return...
		return;
	VoxelBlock& block = getBlock(blockPos);
	auto maxLoad = block.getVoxelBlockMetaData().maxLoad;
	uint32_t dividedStability = block.stability/(neighborCount-1);
	if (dividedStability > maxLoad) {
		dividedStability = maxLoad;
	}
	if (dividedStability == 0)
		return;
	for (uint32_t i = 0; i < neighborCount; i++) {
		if (neighborPositions[i] == parentBlockPos)
			continue;
		Vec2D<uint32_t> neighborVecPos = getVecPosFromBlockPos(neighborPositions[i]);
		stabilityBackBuffer.add(neighborVecPos, dividedStability);
		recursiveRecalculateStabilityForBlock(neighborPositions[i], blockPos);
	}
}