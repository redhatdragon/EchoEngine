#pragma once

/*************************************************************************
				[PUBLIC DOMAIN]

				IO_API

An api intended to help build applications in an os agnostic way.
By forcing the application to only touch the below function
prototypes; the only thing one will need to port their code to a different
platform will be a cooresponding IO_API_ header specific to their platorm.
If one does not exist this helps make it easy to build such a file for
their application's needs.  All they must do is define the function
prototypes with the necessary logic.  It's designed to work with pure C
applications (optionally C++ of course).

BUILD:  Compilation should be very straight forward and easy.  All you must
do is include the right IO_API_ header in IO_API.c which contains your
platform's specific needs.  Then compile as you would other projects.
No dynamic or static libraries involved.  Unless the implimentation requires it.

This API is in ALPHA and will likely change over time to suit broader
uses.  It was designed with game dev in mind but may change over time
to be more general purpose.  It's meant to be a little versitile but
mostly as minimal as possible to avoid "porting hell".

Please feel free to contribute with merge requests or a new IO_API_
implimentations at ____

TODO:
-Decide about using an uint32_t or uint8_t array for the canvas data.
Or have it modifiable.
-Decide on using uint16_t or uint32_t for cWidth and cHeight.
-Should recvPacketUDP return a PacketUDP* through a parameter?
-Should there be 3D support?
-Should there be a way to get things like screen data?
-Should there be a way to get/resize the window outside the canvas?

*************************************************************************/

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
	void drawTexture(const void* texture, int x, int y, int w, int h);
	void recolorTexture(void* texture, uint8_t r, uint8_t g, uint8_t b);
	void* getTexture(const char* fileName);
	//void resizeTexture(void* texture, uint32_t width, uint32_t height);
	void releaseTexture(void* texture);
	void drawBackground(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
	void drawText(const char* str, int x, int y, unsigned int fontWidth);
	void drawRect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
	void drawBloom(int x, int y, int w, int h, int intensity);

	//const uint8_t* getKeyboardState();
	bool getKeyState(char k);
	void getMouseState(uint8_t* leftButton, uint8_t* middleButton, uint8_t* rightButton);
	void getMouseCanvasPos(int* x, int* y);

	//void getWindowSize(unsigned int *width, unsigned int *height);
	//void setWindowSize(unsigned int width, unsigned int height);
	void getCanvasSize(unsigned int* width, unsigned int* height);

	bool getFileData(const char* fileName, uint8_t* dataBuffer, uint32_t startPos, uint32_t numBytes);
	bool getFileText(const char* fileName, char* strBuffer);
	bool writeFileData(const char* fileName, uint8_t* data);
	bool writeFileText(const char* fileName, char* str);
	bool appendToFileData(const char* fileName, uint8_t* data);
	bool appendToFileText(const char* fileName, char* str);
	const char* getDirData();

	float getFPS();
	void setFPS(uint32_t fps);

	bool playAudioFile(const char* fileName, uint8_t loop);
	bool getAudioFromFile(const char* fileName, void* audioBuffer);
	bool playAudio(void* audio);

	bool sendPacketUDP(void* packet);
	struct PacketUDP* recvPacketUDP();

	// TODO: switch ThreadPool to void* or above to struct ptrs
	struct ThreadPool;
	struct ThreadPool* ThreadPoolCreate(uint16_t maxThreadCount);
	void ThreadPoolDestroy(struct ThreadPool* self);
	uint16_t ThreadPoolGetFreeThreadCount(struct ThreadPool* self);
	bool ThreadPoolHasFreeThread(struct ThreadPool* self);
	bool ThreadPoolGiveTask(struct ThreadPool* self, void(*func)(void*), void* param);



	//Define these in your application to be called by the IO_API_ implimentations.
	void appStart();
	void appLoop();
	void appEnd();

#ifdef __cplusplus
}
#endif