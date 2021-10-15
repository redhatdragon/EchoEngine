#pragma once
#include "FlatBuffer.h"

template<typename widthCount, typename heightCount, typename depthCount>
class AStar {
	struct Layer {
		bool nodes[widthCount * heightCount];
	};
	FlatFlaggedBuffer<Layer, depthCount> layers;
public:
};