#pragma once
#include "FlatBuffer.h"
#include "Vec.h"

typedef uint16_t PathfindingSubjectID;
typedef uint32_t PathfindingWaypointID;

template<uint32_t width_count, uint32_t height_count, uint32_t size_per_tile, uint32_t max_per_tile = 16>
class Pathfinding {
	struct Tile {
		FlatBuffer<PathfindingSubjectID, max_per_tile> IDs;
		uint16_t loadIncoming[8];
	};
	struct Layer {
		FlatBuffer<Tile, width_count* height_count> tiles;
		__forceinline void place(PathfindingSubjectID id, const Vec2D<uint32_t>& pos, const Vec2D<uint32_t>& siz) {
			Vec2D<uint32_t> start, end;
			start = pos; start /= size_per_tile;
			end = pos; end += siz; end /= size_per_tile;
			for (uint32_t x = start.x; x < end.x; x++) {
				for (uint32_t y = start.y; y < end.y; y++) {
					if (tiles[x + y * width_count].IDs.count == max_per_tile) {
						std::cout << "Error: PathFinding::Layer::place() failed, tile has too many members" << std::endl;
						throw;
					}
					tiles[x + y * width_count].push(id);
				}
			}
		}
	};
	Layer layer;
	PathfindingSubjectID lastSubject;
public:
	__forceinline void init() {
		lastSubject = 0;  // first subject starts at 1 
	}
	__forceinline PathfindingSubjectID subscribe(const Vec2D<uint32_t>& pos, const Vec2D<uint32_t>& siz) {
		lastSubject++;
		layer.place(lastSubject, pos, siz);
		return lastSubject;
	}
};

int x = sizeof(Pathfinding<1000, 1000, 32>);