#pragma once

/*#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")
#pragma comment(lib, "SDL2_image.lib")
#pragma comment(lib, "SDL2_ttf.lib")*/

/*#pragma comment(lib, "lib/x86/SDL2.lib")
#pragma comment(lib, "lib/x86/SDL2main.lib")
#pragma comment(lib, "lib/x86/SDL2_image.lib")
#pragma comment(lib, "lib/x86/SDL2_ttf.lib")*/


#include "../../IO_API.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "ThreadPool.h"

#include <time.h>
#include <stdio.h>
#include <Windows.h>

#ifdef DIR_TO_DATA 
#error "DIR_TO_DATA already defined"
#else
#define DIR_TO_DATA "../data/"
#endif

struct SDL_Window *window;
struct SDL_Renderer *renderer;
const Uint8* keys;

float FPS;

void drawTexture(const void* texture, int x, int y, int w, int h) {
	struct SDL_Texture* t = texture;
	struct SDL_Rect r = { x, y, w, h };
	SDL_RenderCopy(renderer, t, NULL, &r);
}
void recolorTexture(void* texture, uint8_t r, uint8_t g, uint8_t b) {
	SDL_SetTextureColorMod((SDL_Texture*)texture, r, g, b);
}
void* getTexture(const char* fileName) {
	//struct *t = IMG_LoadTexture(renderer, fileName);  //This should be more optimized but fails in debug mode with magic number issue.
	struct SDL_Surface* s = IMG_Load(fileName);
	return SDL_CreateTextureFromSurface(renderer, s);
}
void releaseTexture(void* texture) {
	SDL_DestroyTexture(texture);
}
void drawText(const char* str, int x, int y, unsigned int fontWidth) {
	TTF_Font* font = TTF_OpenFont(DIR_TO_DATA "fonts/consola.ttf", fontWidth);
	if (font == NULL)
		printf("Error: Can't load font arial.ttf.\n");

	SDL_Color color = { 255, 255, 255 };
	SDL_Surface* surface = TTF_RenderText_Solid(font, str, color);
	surface->refcount++;  //Added to prevent crash in debug
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	int strLen = (int)strlen(str);
	struct SDL_Rect r = { x, y, fontWidth*strLen, fontWidth };
	SDL_RenderCopy(renderer, texture, NULL, &r);


	SDL_DestroyTexture(texture);
	SDL_FreeSurface(surface);

	TTF_CloseFont(font);
}
void drawRect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	//SDL_Color color = { 255, 255, 255 };
	//SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, w, y);
	struct SDL_Rect rect = { x, y, w, h };
	//SDL_RenderCopy(renderer, texture, NULL, &rect);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);
	SDL_RenderFillRect(renderer, &rect);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);


	//SDL_DestroyTexture(texture);
}

bool getKeyState(char k) {
	return keys[k+(SDL_SCANCODE_A - 'a')];
}
void getMouseState(uint8_t* leftButton, uint8_t* middleButton, uint8_t* rightButton) {
	Uint32 mouseState = SDL_GetMouseState(NULL, NULL);
	if (leftButton) {
		*leftButton = false;
		if (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT))
			*leftButton = true;
	}
	if (middleButton) {
		*middleButton = false;
		if (mouseState & SDL_BUTTON(SDL_BUTTON_MIDDLE))
			*middleButton = true;
	}
	if (rightButton) {
		*rightButton = false;
		if (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT))
			*rightButton = true;
	}
}
void getMouseCanvasPos(int* x, int* y) {
	SDL_GetMouseState(x, y);
}

void getCanvasSize(unsigned int* width, unsigned int* height) {
	*width = 1024; *height = 768;
}

const char* getDirData() {
	return DIR_TO_DATA;
}

float getFPS() {
	return FPS;
}
void setFPS(uint32_t fps) {

}

int main(int argc, char *argv[]) {
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	window = SDL_CreateWindow("Some window title", 60, 40, 1024, 768, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	if (IMG_Init(IMG_INIT_PNG) == 0)
		printf("Failed to initialize SDL_Image");
	if (TTF_Init() != 0)
		printf("Failed to initialize SDL_ttf");
	printf("CPU cache line size: %d", SDL_GetCPUCacheLineSize());
	printf("\n");
	printf("CPU core count: %d", SDL_GetCPUCount());
	printf("\n");
	printf("System RAM: %d", SDL_GetSystemRAM());
	printf("\n");
	appStart();



	gameLoop:
	{
		clock_t startTime = clock();
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT)
				goto quit;
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)
				goto quit;
		}
		keys = SDL_GetKeyboardState(NULL);
		SDL_RenderClear(renderer);
		appLoop();

		SDL_RenderPresent(renderer);
		const uint32_t mili_per_frame = 16*2;
		clock_t endTime = clock();
		clock_t differentialTime = endTime - startTime;
		if (differentialTime < mili_per_frame) {
			SDL_Delay(mili_per_frame - differentialTime);
			differentialTime = clock() - startTime;
		}
		FPS = (double)1000/((double)differentialTime);
	} goto gameLoop;

quit:
	appEnd();
	TTF_Quit();
	IMG_Quit();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}