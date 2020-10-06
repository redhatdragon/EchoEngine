#include <IO_API/IO_API.h>
#include <EntityObject.h>
#include <EntityObjectLoader.h>
#include <Widget.h>

Widget* button;

struct WIDGET_TYPE {
    enum {
        BUTTON,
        SLIDER
    };
};
struct ColorRGBA {
    uint8_t r, g, b, a;
};
struct WidgetDataButton {
    void* textureOnPressed;
    void* textureOnReleased;
    ColorRGBA color;
};

void setGreenOnPress(Widget* self) {
    WidgetDataButton* widgetData = (WidgetDataButton*)self->data;
    ColorRGBA c = { 225, 255, 225, 255 };
    widgetData->color = c;
}
void setWhiteOnRelease(Widget* self) {
    WidgetDataButton* widgetData = (WidgetDataButton*)self->data;
    memset(&widgetData->color, 255, sizeof(ColorRGBA));
}
void displayButton(Widget* self) {
    WidgetDataButton* tData = (WidgetDataButton*)self->data;
    static ColorRGBA* color = &tData->color;
    //drawRect(self->x, self->y, self->w, self->h, color->r, color->g, color->b, 255);
    if (tData->textureOnPressed == nullptr) return;
    recolorTexture(tData->textureOnPressed, color->r, color->g, color->b);
    if(self->state == Widget::IDLE || self->state == Widget::RELEASED)
        drawTexture(tData->textureOnPressed, self->x, self->y, self->w, self->h);
    else
        drawTexture(tData->textureOnReleased, self->x, self->y, self->w, self->h);
    recolorTexture(tData->textureOnReleased, 255, 255, 255);
}
void cleanUpButton(Widget* self) {
    WidgetDataButton* widgetData = (WidgetDataButton*)self->data;
    free(widgetData->textureOnPressed);
    free(widgetData->textureOnReleased);
    free(widgetData);
}



void appStart() {
    EntityObject player = EntityObjectLoader::createEntityObjectFromFile(std::string(getDirData())+"entities/Reimu.txt");

    ComponentObject* size = player.getComponent("size");
    ComponentObject* texture = player.getComponent("texture");
    ComponentObject* health = player.getComponent("health");
    uint32_t* arr = size->getArrayInt();
    for (uint32_t i = 0; i < size->getArrayIntLen(); i++)
        std::cout << arr[i] << ' ';
    std::cout << std::endl;
    std::cout << texture->getString() << std::endl;
    std::cout << health->getInt() << std::endl;

    button = Widget::createNew(64, 64, 64, 64);
    //button->onClicked = setGreenOnPress;
    button->onPressed = setGreenOnPress;
    button->onReleased = setWhiteOnRelease;
    button->onDisplay = displayButton;
    button->cleanUpData = cleanUpButton;
    button->id = WIDGET_TYPE::BUTTON;
    button->data = malloc(sizeof(WidgetDataButton));
    if (button->data == NULL) throw;

    ColorRGBA color = { 255, 255, 255, 255 };
    WidgetDataButton tData = { nullptr, nullptr, color };
    tData.textureOnPressed = tData.textureOnReleased = getTexture((std::string(getDirData()) + "textures/RightArrow.png").c_str());
    if (tData.textureOnPressed == nullptr) throw;
    memcpy(button->data, &tData, sizeof(WidgetDataButton));
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