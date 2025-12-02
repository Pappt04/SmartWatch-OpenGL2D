#pragma once
#include "ObjectRenderer.h"
class BatteryObject
{
public:
	unsigned int batteryTexture;
	int percent;

	float batteryWidth = 300.0f;
	float batteryHeight = 500.0f;
	float batteryX;
	float batteryY;

	BatteryObject(unsigned texture,int screenHeight, int screenWidth);
	~BatteryObject();

	void Draw(ObjectRenderer& renderer);
};

