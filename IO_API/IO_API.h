#pragma once

/*
Screen space is in cartesian coordinates.  Relative to window dimensions.
Example: if winWidth and winHeight == 400;  200, 200 is the cnter of the window.
*/

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#ifndef __cplusplus
#ifndef _STDBOOL
#define _STDBOOL
	typedef uint8_t bool;
#define true 1
#define false 0
#endif
#endif



	//void drawTextureFromFile(const char* fileName, int x, int y);
	void* IO_getTexture(const char* fileName);
	//void resizeTexture(void* texture, uint32_t width, uint32_t height);
	void IO_drawTexture(void* texture, int x, int y, int w, int h);
	void IO_recolorTexture(void* texture, uint8_t r, uint8_t g, uint8_t b);
	void IO_releaseTexture(void* texture);
	void IO_drawBackground(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
	void IO_drawText(const char* str, int x, int y, unsigned int fontWidth);
	void IO_drawRect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
	void IO_drawBloom(int x, int y, int w, int h, int intensity);
	void IO_drawPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
	void IO_drawLine(int x1, int y1, int x2, int y2, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

	//const uint8_t* getKeyboardState();
	bool IO_getKeyState(char k);
	void IO_getMouseState(uint8_t* leftButton, uint8_t* middleButton, uint8_t* rightButton);
	void IO_getMouseCanvasPos(int* x, int* y);

	//void getWindowSize(unsigned int *width, unsigned int *height);
	//void setWindowSize(unsigned int width, unsigned int height);
	void IO_getCanvasSize(unsigned int* width, unsigned int* height);

	bool IO_getFileData(const char* fileName, uint8_t* dataBuffer);
	bool IO_getFileText(const char* fileName, char* strBuffer);
	bool IO_writeFileData(const char* fileName, uint8_t* data);
	bool IO_writeFileText(const char* fileName, char* str);
	uint32_t IO_fileGetSize(const char* fileName);
	const char* IO_getDirData();

	float IO_getFPS();
	void IO_setFPS(uint32_t fps);

	bool IO_playAudioFile(const char* fileName, uint8_t loop);
	bool IO_getAudioFromFile(const char* fileName, void* audioBuffer);
	bool IO_playAudio(void* audio);

	bool IO_sendPacketUDP(void* packet);
	struct IO_PacketUDP* IO_recvPacketUDP();

	// TODO: switch ThreadPool to void* or above to struct ptrs
	struct IO_ThreadPool;
	extern struct IO_ThreadPool* IO_threadPool;
	struct IO_ThreadPool* IO_ThreadPoolCreate(uint16_t maxThreadCount);
	void IO_ThreadPoolDestroy(struct IO_ThreadPool* self);
	uint16_t IO_ThreadPoolGetFreeThreadCount(struct IO_ThreadPool* self);
	bool IO_ThreadPoolHasFreeThread(struct IO_ThreadPool* self);
	bool IO_ThreadPoolGiveTask(struct IO_ThreadPool* self, void(*func)(void*), void* param);



	//Define these in your application to be called by the IO_API_ implimentations.
	void IO_appStart();
	void IO_appLoop();
	void IO_appEnd();

#ifdef __cplusplus
}
#endif