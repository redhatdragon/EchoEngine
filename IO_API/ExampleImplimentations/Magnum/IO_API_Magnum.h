#pragma once
#include "../../IO_API.h"
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/Platform/Sdl2Application.h>

#include <time.h>
#include <stdio.h>
using namespace Magnum;

#ifdef DIR_TO_DATA 
#error "DIR_TO_DATA already defined"
#else
#define DIR_TO_DATA "../data/"
#endif

float FPS;
float FPSLimit = 30;

void drawTexture(void* texture, int x, int y, int w, int h) {
	
}
void recolorTexture(void* texture, uint8_t r, uint8_t g, uint8_t b) {
	
}
void* getTexture(const char* fileName) {
	return 0;
}
void releaseTexture(void* texture) {
	
}
void drawText(const char* str, int x, int y, unsigned int fontWidth) {
	
}
void drawRect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	
}
void drawPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	
}
void drawLine(int x1, int y1, int x2, int y2, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	
}

bool getKeyState(char k) {
	return false;
}
void getMouseState(uint8_t* leftButton, uint8_t* middleButton, uint8_t* rightButton) {
	
}
void getMouseCanvasPos(int* x, int* y) {
	
}

void getCanvasSize(unsigned int* width, unsigned int* height) {
	
}

bool getFileText(const char* fileName, char* strBuffer) {
	FILE* t = fopen(fileName, "r");
	if (t == NULL) return false;
	char c;
	size_t i = 0;
	while ((c = getc(t)) != EOF) {
		strBuffer[i] = c;
		i++;
	}
	fclose(t);
	return true;
}
uint32_t fileGetSize(const char* fileName) {
	FILE* t = fopen(fileName, "r");
	if (t == NULL) return 0;
	fseek(t, 0L, SEEK_END);
	uint32_t retValue = ftell(t);
	fseek(t, 0L, SEEK_SET);
	fclose(t);
	return retValue;
}
const char* getDirData() {
	return DIR_TO_DATA;
}

float getFPS() {
	return FPS;
}
void setFPS(uint32_t fps) {
	FPSLimit = fps;
}



class MyApplication : public Platform::Application {
public:
	explicit MyApplication(const Arguments& arguments);

private:
	void drawEvent() override;
};

MyApplication::MyApplication(const Arguments& arguments) :
	Platform::Application{ arguments } {
	appStart();
}

void MyApplication::drawEvent() {
	GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);
	appLoop();
	swapBuffers();
}
MAGNUM_APPLICATION_MAIN(MyApplication)