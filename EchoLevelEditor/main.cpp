#include <IO_API/IO_API.h>
#include <EntityObject.h>
#include <EntityObjectLoader.h>
#include <Widget.h>
#include "UI/UIUtility.h"



void appStart() {
    //createArrowButtonAddSub(parent, x, y, w, h, valueRef, delta, lowest, highest);
    //createTextFieldUint(parent, x, y, w, h, valueRef, lowest, highest);
}
void appLoop() {
    int mx, my;
    getMouseCanvasPos(&mx, &my);
    uint8_t leftMouseButton;
    getMouseState(&leftMouseButton, NULL, NULL);
    Widget::update(mx, my, leftMouseButton);
    Widget::display();
    
    /*void* k = getTexture("data/textures/Koishi.png");
    drawTexture(k, 128, 128, 64, 64);
    drawTexture(k, 128, 128+64, 32, 32);
    drawTexture(k, 128, 128+64+64, 16, 16);
    releaseTexture(k);*/
}
void appEnd() {
	
}