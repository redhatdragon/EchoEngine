#pragma once
#include "TextBox.h"
#include "UIUtility.h"

struct TextBoxIntData {
	WidgetVisualData visualData;
	uint32_t& valueRef;
};

void cleanUpData(Widget* textBoxInt) {
	TextBoxIntData* data = (TextBoxIntData*)textBoxInt->data;
	//free(data->visualData.textureOnPressed);
	//free(data->visualData.textureOnReleased);
	free(data);
}
void createData(Widget* textBoxInt, uint32_t& valueRef) {
	TextBoxIntData* boxData = (TextBoxIntData*)malloc(sizeof(TextBoxIntData));
	boxData->visualData = {};

	boxData->valueRef = valueRef;
	textBoxInt->data = boxData;
}

Widget* createTextBoxInt(Widget* parent, int x, int y, int w, int h, uint32_t& valueRef) {
	Widget* retValue = Widget::createNew(x, y, w, h);
	retValue->parent = parent;
	createData(retValue, valueRef);
	retValue->cleanUpData = cleanUpData;
	return retValue;
}