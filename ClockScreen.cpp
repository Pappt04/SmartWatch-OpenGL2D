#include "ClockScreen.h"
#include "TextRenderer.h"
#include "Util.h"
#include <ctime>

ClockScreen::ClockScreen(int screenWidth, int screenHeight)
    : wWidth(screenWidth), wHeight(screenHeight), lastUpdate(0.0) {
    // Initialize time from system
    time_t now = time(0);
    struct tm ltm;
    localtime_s(&ltm, &now);
    hours = ltm.tm_hour;
    minutes = ltm.tm_min;
    seconds = ltm.tm_sec;
}

void ClockScreen::update(double currentTime) {
    if (currentTime - lastUpdate >= 1.0) {
        lastUpdate = currentTime;
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
}

void ClockScreen::draw(TextRenderer& textRenderer) {
    std::stringstream timeStr;
    timeStr << std::setfill('0') << std::setw(2) << hours << ":"
        << std::setfill('0') << std::setw(2) << minutes << ":"
        << std::setfill('0') << std::setw(2) << seconds;

    float textX = wWidth / 2.0f;
    float textY = wHeight / 2.0f;
    textRenderer.renderText(timeStr.str(), textX, textY, 1.0f, 1.0f, 1.0f, 1.0f);
}