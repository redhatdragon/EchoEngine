//#include "ArrowButton.h"
//#include <Widget.h>
#include "UIUtility.h"

void setGreenOnPress(Widget* self) {
    WidgetVisualData* widgetData = (WidgetVisualData*)self->data;
    WidgetColorRGBA c = { 225, 255, 225, 255 };
    widgetData->color = c;
}
void setWhiteOnRelease(Widget* self) {
    WidgetVisualData* widgetData = (WidgetVisualData*)self->data;
    memset(&widgetData->color, 255, sizeof(WidgetColorRGBA));
}
void displayButton(Widget* self) {
    WidgetVisualData* tData = (WidgetVisualData*)self->data;
    static WidgetColorRGBA* color = &tData->color;
    //drawRect(self->x, self->y, self->w, self->h, color->r, color->g, color->b, 255);
    if (tData->textureOnPressed == nullptr) return;
    recolorTexture(tData->textureOnPressed, color->r, color->g, color->b);
    if (self->state == Widget::IDLE || self->state == Widget::RELEASED)
        drawTexture(tData->textureOnPressed, self->x, self->y, self->w, self->h);
    else
        drawTexture(tData->textureOnReleased, self->x, self->y, self->w, self->h);
    recolorTexture(tData->textureOnReleased, 255, 255, 255);
}
void cleanUpButton(Widget* self) {
    WidgetVisualData* widgetData = (WidgetVisualData*)self->data;
    free(widgetData->textureOnPressed);
    free(widgetData->textureOnReleased);
    free(widgetData);
}

void createArrowButtonAddSub(Widget* parent, int x, int y, int w, int h, uint32_t& valueRef, uint32_t delta,
    uint32_t lowest, uint32_t highest) {
    Widget* button = Widget::createNew(x, y, w, h);
    button->parent = parent;
    //button->onClicked = setGreenOnPress;
    button->onPressed = setGreenOnPress;
    button->onReleased = setWhiteOnRelease;
    button->onDisplay = displayButton;
    button->cleanUpData = cleanUpButton;
    button->id = WIDGET_TYPE::BUTTON;
    button->data = malloc(sizeof(WidgetVisualData));
    if (button->data == NULL) throw;

    WidgetColorRGBA color = { 255, 255, 255, 255 };
    WidgetVisualData tData = { nullptr, nullptr, color };
    tData.textureOnPressed = tData.textureOnReleased = getTexture((std::string(getDirData()) + "textures/RightArrow.png").c_str());
    if (tData.textureOnPressed == nullptr) throw;
    memcpy(button->data, &tData, sizeof(WidgetVisualData));
}