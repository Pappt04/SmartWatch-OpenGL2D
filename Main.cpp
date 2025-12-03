#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Util.h"
#include "ObjectRenderer.h"
#include "TextRenderer.h"
#include "ClockScreen.h"
#include "EkgObject.h"
#include "BatteryObject.h"
#include "ScreenManager.h"

int wWidth = 800, wHeight = 800;
ScreenManager* g_screenManager = nullptr;

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
        g_screenManager->handleClick(xpos, ypos);
    }
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        return endProgram("GLFW failed to initialize");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Get screen resolution
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    wWidth = mode->width;
    wHeight = mode->height;

    std::cout << "Screen resolution detected: " << wWidth << "x" << wHeight << std::endl;

    // Create window
    GLFWwindow* window = glfwCreateWindow(wWidth, wHeight, "SmartWatch2D", monitor, NULL);
    if (window == NULL) {
        return endProgram("Window creation failed");
    }
    glfwMakeContextCurrent(window);

    // Set custom cursor
    GLFWcursor* heartCursor = loadImageToCursor("./res/red_heart.svg.png");
    if (heartCursor != nullptr) {
        glfwSetCursor(window, heartCursor);
    }

    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        return endProgram("GLEW initialization failed");
    }

    // Initialize FreeType
    if (!initFreeType("./fonts/digit_font.ttf", 96)) {
        std::cout << "Warning: Primary font failed, trying alternative..." << std::endl;
        if (!initFreeType("C:/Windows/Fonts/arial.ttf", 96)) {
            return endProgram("FreeType initialization failed");
        }
    }

    // OpenGL settings
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);

    // Create shaders
    unsigned int rectShader = createShader("rect.vert", "rect.frag");
    unsigned int textShader = createShader("text.vert", "text.frag");

    // Set up text projection matrix
    glUseProgram(textShader);
    float textProjection[16] = {
        2.0f / wWidth, 0, 0, 0,
        0, 2.0f / wHeight, 0, 0,
        0, 0, -1, 0,
        -1, -1, 0, 1
    };
    glUniformMatrix4fv(glGetUniformLocation(textShader, "projection"),
        1, GL_FALSE, textProjection);

    // Load textures - only need right arrow, we'll flip it for left
    unsigned arrowRightTexture, warningTexture;
    unsigned ekgTexture, batteryTexture;

    preprocessTexture(arrowRightTexture, "./res/arrow_right.png");
    preprocessTexture(warningTexture, "./res/warning.png");
    preprocessTexture(ekgTexture, "./res/ecg_wave.png");
    preprocessTexture(batteryTexture, "./res/battery.png");

    // Create renderer and screen objects
    ObjectRenderer renderer(rectShader, wWidth, wHeight);
    TextRenderer textRenderer(textShader, wWidth, wHeight);

    ClockScreen clockScreen(wWidth, wHeight);
    EkgObject ekgScreen(ekgTexture, warningTexture, wWidth, wHeight);
    BatteryObject batteryScreen(batteryTexture, wWidth, wHeight);

    // Create screen manager (only pass right arrow, it will flip for left)
    ScreenManager screenManager(wWidth, wHeight, arrowRightTexture);
    screenManager.setClockScreen(&clockScreen);
    screenManager.setEkgScreen(&ekgScreen);
    screenManager.setBatteryScreen(&batteryScreen);
    g_screenManager = &screenManager;

    // Main render loop
    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        bool isRunning = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;

        // Update all screens
        clockScreen.update(currentTime);
        ekgScreen.update(currentTime, isRunning);
        batteryScreen.update(currentTime);

        // Render
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw current screen (includes battery indicator and arrows)
        screenManager.draw(renderer, textRenderer, isRunning);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteProgram(rectShader);
    glDeleteProgram(textShader);
    if (heartCursor) glfwDestroyCursor(heartCursor);
    cleanupFreeType();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}