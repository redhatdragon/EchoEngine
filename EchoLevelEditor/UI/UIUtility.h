#pragma once
#include <Widget.h>
#include <IO_API/IO_API.h>

struct WIDGET_TYPE {
    enum {
        BUTTON,
        SLIDER
    };
};


struct WidgetColorRGBA {
    uint8_t r, g, b, a;
};
struct WidgetVisualData {
    void* textureOnPressed;
    void* textureOnReleased;
    WidgetColorRGBA color;
};

#include "TextBox.h"
#include "ArrowButton.h"
#include "TextFieldUint.h"