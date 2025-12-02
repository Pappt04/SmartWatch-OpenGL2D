#include "BatteryObject.h"

BatteryObject::BatteryObject(unsigned texture, int screenHeight, int screenWidth)
{
	batteryTexture = texture;
	batteryX = (float)(screenWidth - batteryWidth - 50);
	batteryY = (float)(screenHeight - batteryHeight - 50);
	percent = 100;
}

BatteryObject::~BatteryObject()
{

}

void BatteryObject::Draw(ObjectRenderer& renderer)
{
	float filledHeight = (batteryHeight - 20.0f) * (percent / 100.0f);
	float offsetY = batteryY + 10.0f + (batteryHeight - 20.0f - filledHeight);
	renderer.Draw(batteryTexture, glm::vec2(batteryX + 10.0f, offsetY), glm::vec2(batteryWidth - 20.0f, filledHeight));
}
