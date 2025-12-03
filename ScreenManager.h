#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class ObjectRenderer;
class TextRenderer;
class ClockScreen;
class EkgObject;
class BatteryObject;

enum Screen {
    SCREEN_CLOCK,
    SCREEN_HEART_RATE,
    SCREEN_BATTERY
};

class ScreenManager {
private:
    Screen currentScreen;
    int wWidth, wHeight;

    unsigned int arrowRightTexture;  
    ClockScreen* clockScreen;
    EkgObject* ekgScreen;
    BatteryObject* batteryScreen;

    void drawArrows(ObjectRenderer& renderer);
    bool checkArrowClick(double xpos, double ypos, bool isLeft);

public:
    ScreenManager(int screenWidth, int screenHeight, unsigned int rightArrow);
    ~ScreenManager();

    void setClockScreen(ClockScreen* clock) { clockScreen = clock; }
    void setEkgScreen(EkgObject* ekg) { ekgScreen = ekg; }
    void setBatteryScreen(BatteryObject* battery) { batteryScreen = battery; }

    void handleClick(double xpos, double ypos);
    void draw(ObjectRenderer& renderer, TextRenderer& textRenderer, bool isRunning);

    Screen getCurrentScreen() const { return currentScreen; }
};
