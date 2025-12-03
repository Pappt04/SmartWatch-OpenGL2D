#include "ScreenManager.h"
#include "ObjectRenderer.h"
#include "TextRenderer.h"
#include "ClockScreen.h"
#include "EkgObject.h"
#include "BatteryObject.h"

ScreenManager::ScreenManager(int screenWidth, int screenHeight, unsigned int rightArrow)
    : wWidth(screenWidth), wHeight(screenHeight),
    arrowRightTexture(rightArrow),
    currentScreen(SCREEN_CLOCK), clockScreen(nullptr),
    ekgScreen(nullptr), batteryScreen(nullptr) {
}

ScreenManager::~ScreenManager() {
    if (arrowRightTexture != 0) glDeleteTextures(1, &arrowRightTexture);
}

bool ScreenManager::checkArrowClick(double xpos, double ypos, bool isLeft) {
    float arrowX = isLeft ? 50.0f : wWidth - 150.0f;
    float arrowY = wHeight / 2.0f - 50.0f;
    float arrowSize = 100.0f;

    return (xpos >= arrowX && xpos <= arrowX + arrowSize &&
        ypos >= arrowY && ypos <= arrowY + arrowSize);
}

void ScreenManager::handleClick(double xpos, double ypos) {
    switch (currentScreen) {
    case SCREEN_CLOCK:
        if (checkArrowClick(xpos, ypos, false)) {
            currentScreen = SCREEN_HEART_RATE;
        }
        break;

    case SCREEN_HEART_RATE:
        if (checkArrowClick(xpos, ypos, true)) {
            currentScreen = SCREEN_CLOCK;
        }
        else if (checkArrowClick(xpos, ypos, false)) {
            currentScreen = SCREEN_BATTERY;
        }
        break;

    case SCREEN_BATTERY:
        if (checkArrowClick(xpos, ypos, true)) {
            currentScreen = SCREEN_HEART_RATE;
        }
        break;
    }
}

void ScreenManager::drawArrows(ObjectRenderer& renderer) {
    glm::vec2 arrowSize(100.0f, 100.0f);
    float arrowY = wHeight / 2.0f - 50.0f;

    switch (currentScreen) {
    case SCREEN_CLOCK:
        // Right arrow only
        renderer.Draw(arrowRightTexture,
            glm::vec2(wWidth - 150.0f, arrowY),
            arrowSize, 0.0f);
        break;

    case SCREEN_HEART_RATE:
        // Left arrow (flipped)
        renderer.DrawFlipped(arrowRightTexture,
            glm::vec2(50.0f, arrowY),
            arrowSize, true, false, 0.0f);
        // Right arrow
        renderer.Draw(arrowRightTexture,
            glm::vec2(wWidth - 150.0f, arrowY),
            arrowSize, 0.0f);
        break;

    case SCREEN_BATTERY:
        // Left arrow (flipped)
        renderer.DrawFlipped(arrowRightTexture,
            glm::vec2(50.0f, arrowY),
            arrowSize, true, false, 0.0f);
        break;
    }
}

void ScreenManager::drawWatchBezel(ObjectRenderer& renderer) {
    // Draw circular smartwatch bezel in the center of the screen
    float centerX = wWidth / 2.0f;
    float centerY = wHeight / 2.0f;
    float watchRadius = 350.0f;  // Radius of the watch face
    
    // Draw outer bezel (dark gray)
    glm::vec3 bezelColor(0.2f, 0.2f, 0.2f);
    renderer.DrawCircle(glm::vec2(centerX, centerY), watchRadius + 15.0f, bezelColor, 100);
    
    // Draw inner watch face (very dark, almost black)
    glm::vec3 watchFaceColor(0.05f, 0.05f, 0.08f);
    renderer.DrawCircle(glm::vec2(centerX, centerY), watchRadius, watchFaceColor, 100);
}

void ScreenManager::draw(ObjectRenderer& renderer, TextRenderer& textRenderer, bool isRunning) {
    // Draw the circular watch bezel first
    drawWatchBezel(renderer);
    
    // Draw arrows
    drawArrows(renderer);

    // Draw battery indicator on all screens except battery screen
    if (currentScreen != SCREEN_BATTERY && batteryScreen) {
        batteryScreen->drawIndicator(renderer, textRenderer);
    }

    // Draw current screen content
    switch (currentScreen) {
    case SCREEN_CLOCK:
        if (clockScreen) {
            clockScreen->draw(textRenderer);
        }
        break;

    case SCREEN_HEART_RATE:
        if (ekgScreen) {
            ekgScreen->draw(renderer, textRenderer);
        }
        break;

    case SCREEN_BATTERY:
        if (batteryScreen) {
            batteryScreen->draw(renderer, textRenderer);
        }
        break;
    }
}