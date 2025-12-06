#include "ScreenManager.h"
#include "ObjectRenderer.h"

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
    float centerX = wWidth / 2.0f;
    float centerY = wHeight / 2.0f;
    float watchRadius = 350.0f;
    
    float arrowSize = 80.0f;  
    float arrowOffset = watchRadius - arrowSize - 20.0f;
    
    float arrowX = isLeft ? (centerX - arrowOffset - arrowSize) : (centerX + arrowOffset);
    float arrowY = centerY - arrowSize / 2.0f;

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
    float centerX = wWidth / 2.0f;
    float centerY = wHeight / 2.0f;
    float watchRadius = 350.0f;
    
    float arrowSize = 80.0f;
    float arrowOffset = watchRadius - arrowSize - 20.0f;
    
    glm::vec2 arrowSizeVec(arrowSize, arrowSize);
    float arrowY = centerY - arrowSize / 2.0f;

    switch (currentScreen) {
    case SCREEN_CLOCK:
        // Right arrow only
        renderer.Draw(arrowRightTexture,
            glm::vec2(centerX + arrowOffset, arrowY),
            arrowSizeVec, 0.0f);
        break;

    case SCREEN_HEART_RATE:
        // Left arrow
        renderer.DrawFlipped(arrowRightTexture,
            glm::vec2(centerX - arrowOffset - arrowSize, arrowY),
            arrowSizeVec, true, false, 0.0f);
        // Right arrow
        renderer.Draw(arrowRightTexture,
            glm::vec2(centerX + arrowOffset, arrowY),
            arrowSizeVec, 0.0f);
        break;

    case SCREEN_BATTERY:
        // Left arrow
        renderer.DrawFlipped(arrowRightTexture,
            glm::vec2(centerX - arrowOffset - arrowSize, arrowY),
            arrowSizeVec, true, false, 0.0f);
        break;
    }
}

void ScreenManager::drawWatchBezel(ObjectRenderer& renderer) {
    float centerX = wWidth / 2.0f;
    float centerY = wHeight / 2.0f;
    float watchRadius = 350.0f;
    
    // Draw outer bezel (dark gray)
    glm::vec3 bezelColor(0.2f, 0.2f, 0.2f);
    renderer.DrawCircle(glm::vec2(centerX, centerY), watchRadius + 15.0f, bezelColor, 100);
    
    // Draw inner watch face (very dark)
    glm::vec3 watchFaceColor(0.05f, 0.05f, 0.08f);
    renderer.DrawCircle(glm::vec2(centerX, centerY), watchRadius, watchFaceColor, 100);
}

void ScreenManager::draw(ObjectRenderer& renderer, TextRenderer& textRenderer, bool isRunning) {
    drawWatchBezel(renderer);
    
    drawArrows(renderer);

	studentInfo->draw(renderer, textRenderer);

    if (currentScreen != SCREEN_BATTERY && batteryScreen) {
        batteryScreen->drawIndicator(renderer, textRenderer);
    }

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