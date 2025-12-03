#pragma once
#include <string>
#include <sstream>
#include <iomanip>

class TextRenderer;
class ObjectRenderer;

class ClockScreen {
private:
    int hours, minutes, seconds;
    double lastUpdate;
    int wWidth, wHeight;

public:
    ClockScreen(int screenWidth, int screenHeight);
    void update(double currentTime);
    void draw(TextRenderer& textRenderer);

    // Getters
    int getHours() const { return hours; }
    int getMinutes() const { return minutes; }
    int getSeconds() const { return seconds; }
};