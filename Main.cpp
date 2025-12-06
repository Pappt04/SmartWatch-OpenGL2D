#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Util.h"
#include "ObjectRenderer.h"
#include "TextRenderer.h"
#include "ClockScreen.h"
#include "EkgObject.h"
#include "BatteryObject.h"
#include "StudentInfoObject.h"
#include "ScreenManager.h"
#include <chrono>
#include <thread>

#define TARGET_FPS 75

int wWidth = 800, wHeight = 800;
ScreenManager* g_screenManager = nullptr;

GLFWcursor* basicCursor = nullptr;
GLFWcursor* pressedCursor = nullptr;

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

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && g_screenManager) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        glfwSetCursor(window, pressedCursor);
        g_screenManager->handleClick(xpos, ypos);
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        glfwSetCursor(window, basicCursor);
    }
}

int main() {
    if (!glfwInit()) {
        return endProgram("GLFW failed to initialize");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    wWidth = mode->width;
    wHeight = mode->height;

    std::cout << "Screen resolution detected: " << wWidth << "x" << wHeight << std::endl;

    GLFWwindow* window = glfwCreateWindow(wWidth, wHeight, "SmartWatch2D", monitor, NULL);
    if (window == NULL) {
        return endProgram("Window creation failed");
    }
    glfwMakeContextCurrent(window);

    basicCursor = loadImageToCursor("./res/red_heart_cursor.png");
    pressedCursor = loadImageToCursor("./res/red_heart_dark_cursor.png");
    if (basicCursor != nullptr) {
        glfwSetCursor(window, basicCursor);
    }

    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    if (glewInit() != GLEW_OK) {
        return endProgram("GLEW initialization failed");
    }

    if (!initFreeType("./fonts/digit_font.ttf", 96)) {
        std::cout << "Warning: Primary font failed, trying alternative..." << std::endl;
        if (!initFreeType("C:/Windows/Fonts/arial.ttf", 96)) {
            return endProgram("FreeType initialization failed");
        }
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);

    unsigned int rectShader = createShader("rect.vert", "rect.frag");
    unsigned int textShader = createShader("text.vert", "text.frag");

    glUseProgram(textShader);
    float textProjection[16] = {
        2.0f / wWidth, 0, 0, 0,
        0, 2.0f / wHeight, 0, 0,
        0, 0, -1, 0,
        -1, -1, 0, 1
    };
    glUniformMatrix4fv(glGetUniformLocation(textShader, "projection"),
        1, GL_FALSE, textProjection);

    unsigned arrowRightTexture, warningTexture;
    unsigned ekgTexture, batteryTexture;

    preprocessTexture(arrowRightTexture, "./res/arrow_right.png");
    preprocessTexture(warningTexture, "./res/warning.png");
    preprocessTexture(ekgTexture, "./res/ecg_wave.png");
    preprocessTexture(batteryTexture, "./res/battery.png");

    ObjectRenderer renderer(rectShader, wWidth, wHeight);
    TextRenderer textRenderer(textShader, wWidth, wHeight);

    ClockScreen clockScreen(wWidth, wHeight);
    EkgObject ekgScreen(ekgTexture, warningTexture, wWidth, wHeight);
    BatteryObject batteryScreen(batteryTexture, wWidth, wHeight);
    StudentInfoObject studentInfo("Tamas Papp","RA-4-2022", wWidth, wHeight);

    ScreenManager screenManager(wWidth, wHeight, arrowRightTexture);
    screenManager.setClockScreen(&clockScreen);
    screenManager.setEkgScreen(&ekgScreen);
    screenManager.setBatteryScreen(&batteryScreen);
	screenManager.setStudentInfo(&studentInfo);
    g_screenManager = &screenManager;


    auto lastFrameTime = std::chrono::high_resolution_clock::now();
    const double frameTime = 1.0 / TARGET_FPS;

    // Main render loop
    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        bool isRunning = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
		bool shouldClose = glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS;
        if (shouldClose) {
            glfwSetWindowShouldClose(window, true);
        }

        clockScreen.update(currentTime);
        ekgScreen.update(currentTime, isRunning);
        batteryScreen.update(currentTime);

        glClear(GL_COLOR_BUFFER_BIT);

        screenManager.draw(renderer, textRenderer, isRunning);

        glfwSwapBuffers(window);
        glfwPollEvents();

		// Frame limiter
        std::chrono::duration<double> elapsedTime = std::chrono::high_resolution_clock::now() - lastFrameTime;
        auto sleepTime = std::chrono::duration<double>(frameTime) - elapsedTime;
        if (sleepTime.count() > 0.0) {
            std::this_thread::sleep_for(sleepTime);
        }
        lastFrameTime = std::chrono::high_resolution_clock::now();
    }

    glDeleteProgram(rectShader);
    glDeleteProgram(textShader);
    if (basicCursor) glfwDestroyCursor(basicCursor);
    if (pressedCursor) glfwDestroyCursor(pressedCursor);
    cleanupFreeType();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}