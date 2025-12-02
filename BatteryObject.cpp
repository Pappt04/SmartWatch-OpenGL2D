#include "BatteryObject.h"
#include "Util.h"
#include <algorithm>

extern int wWidth, wHeight;

BatteryObject::BatteryObject(unsigned texture, int screenWidth, int screenHeight)
{
	wWidth = screenWidth;
	wHeight = screenHeight;
	percent = 100;
	lastUpdate = 0.0;
	batteryHeight = 300.0f;
	batteryWidth = 200.0f;

	batteryX = batteryWidth + 50.f;
	batteryY = wHeight - 50.0f;

	batteryTexture = texture;
}

BatteryObject::~BatteryObject() {
	if (batteryTexture != 0) {
		glDeleteTextures(1, &batteryTexture);
	}
}

void BatteryObject::update(double currentTime) {
	// Update battery drain every 10 seconds
	if (currentTime - lastUpdate >= 0.2) {
		lastUpdate = currentTime;
		percent = std::max(0, percent - 1);
	}
}

void BatteryObject::drawColoredRect(unsigned int shader, unsigned int VAO,
	float x, float y, float width, float height,
	float r, float g, float b) {
	glUseProgram(shader);

	float left = 0.0f;
	float right = (float)wWidth;
	float bottom = 0.0f;
	float top = (float)wHeight;

	float projection[16] = {
		2.0f / (right - left), 0, 0, 0,
		0, 2.0f / (top - bottom), 0, 0,
		0, 0, -1, 0,
		-(right + left) / (right - left), -(top + bottom) / (top - bottom), 0, 1
	};

	float model[16] = {
		width, 0, 0, 0,
		0, height, 0, 0,
		0, 0, 1, 0,
		x, y, 0, 1
	};

	glUniformMatrix4fv(glGetUniformLocation(shader, "uProjection"), 1, GL_FALSE, projection);
	glUniformMatrix4fv(glGetUniformLocation(shader, "uModel"), 1, GL_FALSE, model);
	glUniform3f(glGetUniformLocation(shader, "uColor"), r, g, b);
	glUniform1i(glGetUniformLocation(shader, "uUseTexture"), 0);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void BatteryObject::draw(unsigned int shader, unsigned int VAO) {
	if (batteryTexture == 0) return;

	glUseProgram(shader);
	glBindTexture(GL_TEXTURE_2D, batteryTexture);

	// Projection matrix
	float left = 0.0f;
	float right = (float)wWidth;
	float bottom = 0.0f;
	float top = (float)wHeight;

	float projection[16] = {
		2.0f / (right - left), 0, 0, 0,
		0, 2.0f / (top - bottom), 0, 0,
		0, 0, -1, 0,
		-(right + left) / (right - left), -(top + bottom) / (top - bottom), 0, 1
	};

	// Draw battery outline
	float model[16] = {
		batteryWidth, 0, 0, 0,
		0, batteryHeight, 0, 0,
		0, 0, 1, 0,
		batteryX, batteryY, 0, 1
	};

	glUniformMatrix4fv(glGetUniformLocation(shader, "uProjection"), 1, GL_FALSE, projection);
	glUniformMatrix4fv(glGetUniformLocation(shader, "uModel"), 1, GL_FALSE, model);
	glUniform1i(glGetUniformLocation(shader, "uUseTexture"), 1);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	// Draw battery fill
	float fillHeight = (batteryHeight - 60.0f);// *(percent / 100.0f);
	float fillWidth = (batteryWidth - 60.0f) * (percent / 100.f);
	float fillX = batteryX + batteryWidth + 30.0f - fillWidth;
	float fillY = 0.f;// batteryY - 40.0f;

	// Set color based on percentage
	if (percent > 20) {
		drawColoredRect(shader, VAO, fillX, fillY, fillWidth, fillHeight, 0.0f, 1.0f, 0.0f);
	}
	else if (percent > 10) {
		drawColoredRect(shader, VAO, fillX, fillY, fillWidth, fillHeight, 1.0f, 1.0f, 0.0f);
	}
	else {
		drawColoredRect(shader, VAO, fillX, fillY, fillWidth, fillHeight, 1.0f, 0.0f, 0.0f);
	}
}
