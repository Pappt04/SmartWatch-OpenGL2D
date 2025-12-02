#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>
#include <iostream>
#include <string>
#include <ctime>
#include <random>
#include <sstream>
#include <iomanip>
#include "Util.h"

// Screen states
enum Screen {
	SCREEN_CLOCK,
	SCREEN_HEART_RATE,
};

// Global variables
Screen currentScreen = SCREEN_HEART_RATE;
int wWidth = 800, wHeight = 800;
int hours = 12, minutes = 0, seconds = 0;
double lastTimeUpdate = 0.0;
int heartRate = 70;
double lastHeartRateUpdate = 0.0;
bool isRunning = false;
float ekgScrollOffset = 0.0f;
float ekgScale = 1.0f;
int batteryPercent = 100;
double lastBatteryUpdate = 0.0;

// Random generator
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> heartDist(60, 80);

int endProgram(std::string message) {
	std::cout << message << std::endl;
	cleanupFreeType();
	glfwTerminate();
	return -1;
}

void preprocessTexture(unsigned& texture, const char* filepath) {
	texture = loadImageToTexture(filepath);
	if (texture == 0) return;

	glBindTexture(GL_TEXTURE_2D, texture);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void drawTexturedRect(unsigned int shader, unsigned int VAO, unsigned int texture,
	float x, float y, float width, float height,
	float texOffsetX = 0.0f, float texScaleX = 1.0f) {
	if (texture == 0) return;

	glUseProgram(shader);
	glBindTexture(GL_TEXTURE_2D, texture);

	// Projection matrix (screen coordinates)
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

	// Model matrix
	float model[16] = {
		width, 0, 0, 0,
		0, height, 0, 0,
		0, 0, 1, 0,
		x, y, 0, 1
	};

	glUniformMatrix4fv(glGetUniformLocation(shader, "uProjection"), 1, GL_FALSE, projection);
	glUniformMatrix4fv(glGetUniformLocation(shader, "uModel"), 1, GL_FALSE, model);
	glUniform1f(glGetUniformLocation(shader, "uTexOffsetX"), texOffsetX);
	glUniform1f(glGetUniformLocation(shader, "uTexScaleX"), texScaleX);
	glUniform1i(glGetUniformLocation(shader, "uUseTexture"), 1);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void drawColoredRect(unsigned int shader, unsigned int VAO,
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

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		// Arrow regions (approximately)
		float leftArrowX = 50.0f;
		float rightArrowX = wWidth - 100.0f;
		float arrowY = wHeight / 2.0f - 50.0f;
		float arrowSize = 100.0f;

		if (currentScreen == SCREEN_CLOCK) {
			// Right arrow
			if (xpos >= rightArrowX && xpos <= rightArrowX + arrowSize &&
				ypos >= arrowY && ypos <= arrowY + arrowSize) {
				currentScreen = SCREEN_HEART_RATE;
			}
		}
		else if (currentScreen == SCREEN_HEART_RATE) {
			// Left arrow
			if (xpos >= leftArrowX && xpos <= leftArrowX + arrowSize &&
				ypos >= arrowY && ypos <= arrowY + arrowSize) {
				currentScreen = SCREEN_CLOCK;
			}
			// Right arrow
			if (xpos >= rightArrowX && xpos <= rightArrowX + arrowSize &&
				ypos >= arrowY && ypos <= arrowY + arrowSize) {
				currentScreen = SCREEN_CLOCK;
			}
		}
	}
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	wWidth = mode->width;
	wHeight = mode->height;

	GLFWwindow* window = glfwCreateWindow(wWidth, wHeight, "SmartWatch2D", monitor, NULL);
	if (window == NULL) return endProgram("Prozor nije uspeo da se kreira.");
	glfwMakeContextCurrent(window);

	GLFWcursor* heartCursor = loadImageToCursor("./res/red_heart.svg.png");
	if (heartCursor != nullptr) {
		glfwSetCursor(window, heartCursor);
	}

	glfwSetMouseButtonCallback(window, mouseButtonCallback);

	if (glewInit() != GLEW_OK) return endProgram("GLEW nije uspeo da se inicijalizuje.");

	// Initialize FreeType
	if (!initFreeType("./fonts/digit_font.ttf", 96)) {
		std::cout << "Warning: FreeType failed to initialize, trying alternative font..." << std::endl;
		if (!initFreeType("C:/Windows/Fonts/arial.ttf", 96)) {
			return endProgram("Failed to initialize FreeType with any font!");
		}
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.1f, 0.1f, 0.15f, 1.0f);

	// Create shaders
	unsigned int rectShader = createShader("rect.vert", "rect.frag");
	unsigned int textShader = createShader("text.vert", "text.frag");

	// Set up text projection matrix (screen coordinates)
	glUseProgram(textShader);
	float textProjection[16] = {
		2.0f / wWidth, 0, 0, 0,
		0, 2.0f / wHeight, 0, 0,
		0, 0, -1, 0,
		-1, -1, 0, 1
	};
	glUniformMatrix4fv(glGetUniformLocation(textShader, "projection"), 1, GL_FALSE, textProjection);

	// Create quad VAO for rectangles
	float quadVertices[] = {
		-0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.0f, 1.0f
	};

	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Load textures
	unsigned int arrowLeftTexture, arrowRightTexture;
	unsigned int ekgTexture, batteryOutlineTexture, warningTexture;

	preprocessTexture(arrowLeftTexture, "./res/arrow_left.png");
	preprocessTexture(arrowRightTexture, "./res/arrow_right.png");
	preprocessTexture(ekgTexture, "./res/ecg_wave.png");
	preprocessTexture(batteryOutlineTexture, "./res/battery.png");
	preprocessTexture(warningTexture, "./res/warning.png");

	// Initialize time
	time_t now = time(0);
	struct tm ltm;
	localtime_s(&ltm, &now);
	hours = ltm.tm_hour;
	minutes = ltm.tm_min;
	seconds = ltm.tm_sec;

	lastTimeUpdate = glfwGetTime();
	lastHeartRateUpdate = glfwGetTime();
	lastBatteryUpdate = glfwGetTime();

	while (!glfwWindowShouldClose(window))
	{
		double currentTime = glfwGetTime();

		// Update clock
		if (currentTime - lastTimeUpdate >= 1.0) {
			lastTimeUpdate = currentTime;
			seconds++;
			if (seconds >= 60) {
				seconds = 0;
				minutes++;
				if (minutes >= 60) {
					minutes = 0;
					hours++;
					if (hours >= 24) {
						hours = 0;
					}
				}
			}
		}

		// Check if running (D key pressed)
		isRunning = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;

		// Update heart rate
		if (currentTime - lastHeartRateUpdate >= 0.1) {
			lastHeartRateUpdate = currentTime;

			if (isRunning) {
				heartRate = std::min(220, heartRate + 2);
				ekgScale = std::max(0.3f, ekgScale - 0.02f);
			}
			else {
				if (heartRate > 70) {
					heartRate = std::max(70, heartRate - 1);
				}
				else {
					heartRate = heartDist(gen);
				}
				ekgScale = std::min(1.0f, ekgScale + 0.01f);
			}
		}

		// Update EKG scroll
		ekgScrollOffset += 0.01f * (heartRate / 70.0f);
		if (ekgScrollOffset > 1.0f) ekgScrollOffset -= 1.0f;

		// Update battery
		if (currentTime - lastBatteryUpdate >= 0.1) {
			lastBatteryUpdate = currentTime;
			batteryPercent = std::max(0, batteryPercent - 1);
		}

		glClear(GL_COLOR_BUFFER_BIT);

		// Render based on current screen
		if (currentScreen == SCREEN_CLOCK) {
			// Draw time HH:MM:SS using FreeType
			std::stringstream timeStr;
			timeStr << std::setfill('0') << std::setw(2) << hours << ":"
				<< std::setfill('0') << std::setw(2) << minutes << ":"
				<< std::setfill('0') << std::setw(2) << seconds;

			float textX = wWidth / 2.0f - 200.0f;
			float textY = wHeight / 2.0f + 30.0f;
			renderText(textShader, timeStr.str(), textX, textY, 1.0f, 1.0f, 1.0f, 1.0f);

			// Draw right arrow
			drawTexturedRect(rectShader, VAO, arrowRightTexture,
				wWidth - 100.0f, wHeight / 2.0f - 50.0f, 100.0f, 100.0f);
		}
		else if (currentScreen == SCREEN_HEART_RATE) {
			// Draw left arrow
			drawTexturedRect(rectShader, VAO, arrowLeftTexture,
				50.0f, wHeight / 2.0f - 50.0f, 100.0f, 100.0f);

			// Draw right arrow
			drawTexturedRect(rectShader, VAO, arrowRightTexture,
				wWidth - 100.0f, wHeight / 2.0f - 50.0f, 100.0f, 100.0f);

			// Draw heart rate number
			std::string bpmText = std::to_string(heartRate) + " BPM";
			float textX = wWidth / 2.0f - 120.0f;
			float textY = wHeight / 2.0f + 200.0f;
			renderText(textShader, bpmText, textX, textY, 1.2f, 1.0f, 0.3f, 0.3f);

			// Draw EKG wave
			float ekgWidth = 600.0f * ekgScale;
			float ekgHeight = 150.0f;
			float ekgX = wWidth / 2.0f - ekgWidth / 2.0f;
			float ekgY = wHeight / 2.0f - ekgHeight / 2.0f;
			drawTexturedRect(rectShader, VAO, ekgTexture,
				ekgX, ekgY, ekgWidth, ekgHeight, ekgScrollOffset, ekgScale);

			// Draw warning if heart rate > 200
			if (heartRate > 200) {
				drawTexturedRect(rectShader, VAO, warningTexture,
					wWidth / 2.0f - 300.0f, wHeight / 2.0f - 300.0f,
					600.0f, 600.0f);

				renderText(textShader, "STOP! ODMORI SE!",
					wWidth / 2.0f - 250.0f, wHeight / 2.0f - 50.0f,
					1.2f, 1.0f, 0.0f, 0.0f);
			}
		}
		// Draw left arrow
		drawTexturedRect(rectShader, VAO, arrowLeftTexture,
			50.0f, wHeight / 2.0f - 50.0f, 100.0f, 100.0f);

		// Draw battery outline
		float batteryWidth = 300.0f;
		float batteryHeight = 500.0f;
		float batteryX = wWidth / 2.0f - batteryWidth / 2.0f;
		float batteryY = wHeight / 2.0f - batteryHeight / 2.0f;
		drawTexturedRect(rectShader, VAO, batteryOutlineTexture,
			batteryX, batteryY, batteryWidth, batteryHeight);

		// Draw battery fill
		float fillHeight = (batteryHeight - 60.0f) * (batteryPercent / 100.0f);
		float fillWidth = batteryWidth - 60.0f;
		float fillX = batteryX + 30.0f;
		float fillY = batteryY + batteryHeight - 40.0f - fillHeight;

		// Set color based on percentage
		if (batteryPercent > 20) {
			drawColoredRect(rectShader, VAO, fillX, fillY, fillWidth, fillHeight, 0.0f, 1.0f, 0.0f);
		}
		else if (batteryPercent > 10) {
			drawColoredRect(rectShader, VAO, fillX, fillY, fillWidth, fillHeight, 1.0f, 1.0f, 0.0f);
		}
		else {
			drawColoredRect(rectShader, VAO, fillX, fillY, fillWidth, fillHeight, 1.0f, 0.0f, 0.0f);
		}

		// Draw battery percentage
		std::string percentText = std::to_string(batteryPercent) + "%";
		float textX = wWidth / 2.0f - 80.0f;
		float textY = wHeight / 2.0f - 350.0f;
		renderText(textShader, percentText, textX, textY, 1.5f, 1.0f, 1.0f, 1.0f);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(rectShader);
	glDeleteProgram(textShader);

	if (heartCursor) glfwDestroyCursor(heartCursor);
	cleanupFreeType();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}