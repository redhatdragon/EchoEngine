#pragma once
#include <string>

void setGreenOnPress(Widget* self);
void setWhiteOnRelease(Widget* self);
void displayButton(Widget* self);
void cleanUpButton(Widget* self);

void createArrowButtonAddSub(Widget* parent, int x, int y, int w, int h, uint32_t& valueRef, uint32_t delta,
    uint32_t lowest = 0, uint32_t highest = -1);