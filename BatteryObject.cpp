#include "BatteryObject.h"
#include "ObjectRenderer.h"
#include "TextRenderer.h"
#include <algorithm>
#include <string>

extern int wWidth, wHeight;

BatteryObject::BatteryObject(unsigned int texture, int screenWidth, int screenHeight)
	: batteryTexture(texture), wWidth(screenWidth), wHeight(screenHeight),
	percent(100), lastUpdate(0.0) {
	batteryWidth = 300.0f;
	batteryHeight = 150.0f;
	batteryX = wWidth / 2.0f - batteryWidth / 2.0f;
	batteryY = wHeight / 2.0f - batteryHeight / 2.0f;
}

BatteryObject::~BatteryObject() {
	if (batteryTexture != 0) {
		glDeleteTextures(1, &batteryTexture);
	}
}

void BatteryObject::update(double currentTime) {
	if (currentTime - lastUpdate >= 10.0) {
		lastUpdate = currentTime;
		percent = std::max(0, percent - 1);
	}
}

void BatteryObject::drawColoredRect(ObjectRenderer& renderer, float x, float y,
	float width, float height, float r, float g, float b) {
	glm::vec2 pos(x, y);
	glm::vec2 size(width, height);
	glm::vec3 color(r, g, b);
	renderer.DrawColored(pos, size, color, 0.0f);
}

void BatteryObject::drawIndicator(ObjectRenderer& renderer, TextRenderer& textRenderer) {
	if (batteryTexture == 0) return;

	// Draw small horizontal battery icon in top-right corner
	float smallBatteryWidth = batteryWidth + 50.f;//120.0f;
	float smallBatteryHeight = batteryHeight + 50.f;//60.0f;
	float smallBatteryX = wWidth - smallBatteryWidth - 80.0f;
	float smallBatteryY = wHeight - smallBatteryHeight - 20.0f;

	glm::vec2 batteryPos(smallBatteryX, smallBatteryY);
	glm::vec2 batterySize(smallBatteryWidth, smallBatteryHeight);
	renderer.Draw(batteryTexture, batteryPos, batterySize, 0.0f);

	// Draw battery fill (left to right)
	float fillPadding = 12.0f;
	float fillWidth = (smallBatteryWidth - 2.0f * fillPadding) * (percent / 100.0f);
	float fillHeight = smallBatteryHeight - 2.0f * fillPadding;
	float fillX = smallBatteryX + fillPadding;
	float fillY = smallBatteryY + fillPadding;

	// Set color based on percentage
	glm::vec3 fillColor;
	if (percent > 20) {
		fillColor = glm::vec3(0.0f, 1.0f, 0.0f);  // Green
	}
	else if (percent > 10) {
		fillColor = glm::vec3(1.0f, 1.0f, 0.0f);  // Yellow
	}
	else {
		fillColor = glm::vec3(1.0f, 0.0f, 0.0f);  // Red
	}

	drawColoredRect(renderer, fillX, fillY, fillWidth, fillHeight,
		fillColor.r, fillColor.g, fillColor.b);

	// Draw percentage text next to battery
	std::string percentText = std::to_string(percent) + "%";
	float textX = smallBatteryX - 60.0f;
	float textY = smallBatteryY - 60.0f;
	textRenderer.renderText(percentText, textX, textY, 0.5f, 1.0f, 1.0f, 1.0f);
}

void BatteryObject::draw(ObjectRenderer& renderer, TextRenderer& textRenderer) {
	if (batteryTexture == 0) return;

	// Draw large horizontal battery in center (for battery screen)
	glm::vec2 batteryPos(batteryX, batteryY);
	glm::vec2 batterySize(batteryWidth, batteryHeight);
	renderer.Draw(batteryTexture, batteryPos, batterySize, 0.0f);

	// Draw battery fill (left to right)
	float fillPadding = 30.0f;
	float fillWidth = (batteryWidth - 2.0f * fillPadding) * (percent / 100.0f);
	float fillHeight = batteryHeight - 2.0f * fillPadding;
	float fillX = batteryX + fillPadding;
	float fillY = batteryY + fillPadding;

	// Set color based on percentage
	glm::vec3 fillColor;
	if (percent > 20) {
		fillColor = glm::vec3(0.0f, 1.0f, 0.0f);  // Green
	}
	else if (percent > 10) {
		fillColor = glm::vec3(1.0f, 1.0f, 0.0f);  // Yellow
	}
	else {
		fillColor = glm::vec3(1.0f, 0.0f, 0.0f);  // Red
	}

	drawColoredRect(renderer, fillX, fillY, fillWidth, fillHeight,
		fillColor.r, fillColor.g, fillColor.b);

	// Draw battery percentage text above battery
	std::string percentText = std::to_string(percent) + "%";
	float textX = wWidth / 2.0f - 80.0f;
	float textY = batteryY + batteryHeight + 50.0f;
	textRenderer.renderText(percentText, textX, textY, 1.5f, 1.0f, 1.0f, 1.0f);
}