#pragma once
#include "FlatBuffer.h"
#include "Vec.h"

typedef uint32_t AStarSubjectID;
typedef uint32_t AStarWaypointID;

template<uint32_t widthCount, uint32_t heightCount, uint32_t sizePerNode>
class AStar {
	struct Layer {
		FlatBuffer<FlatBuffer<AStarSubjectID, 32>, widthCount * heightCount> hashes;
		__forceinline void place(AStarSubjectID id, const Vec2D<uint32_t>& pos, const Vec2D<uint32_t>& siz) {
			Vec2D<uint32_t> start, end;
			start = pos; start /= sizePerNode;
			end = pos; end += siz; end /= sizePerNode;
			for (uint32_t x = start.x; x < end.x; x++) {
				for (uint32_t y = start.y; y < end.y; y++) {
					if (hashes[x + y * widthCount].count == 32) {
						std::cout << "Error: AStar::Layer::place() failed, hash has too many members" << std::endl;
						throw;
					}
					hashes[x + y * widthCount].push(id);
				}
			}
		}
	};
	Layer layer;
	AStarSubjectID lastSubject;
public:
	__forceinline void init() {
		lastSubject = 0;  // first subject starts at 1 
	}
	__forceinline AStarSubjectID subscribe(const Vec2D<uint32_t>& pos, const Vec2D<uint32_t>& siz) {
		lastSubject++;
		layer.place(lastSubject, pos, siz);
		return lastSubject;
	}
};