#pragma once
#include "IO_API.h"

#ifdef __cplusplus
extern "C" {
#endif

struct IO_ColorR8G8B8 {
	uint8_t buffer[3];
};
struct IO_Point2Df {
	float x, y;
};

void IO_drawTriangle2DRGB(const IO_Point2Df points[3], IO_ColorR8G8B8 color);
void IO_drawTriangle2DRGBVertex(const IO_Point2Df points[3], const IO_ColorR8G8B8 colors[3]);
void* IO_getMesh(const char* filepath);
void IO_drawMesh(void* mesh);
void IO_releaseMesh(void* mesh);

#ifdef __cplusplus
}
#endif