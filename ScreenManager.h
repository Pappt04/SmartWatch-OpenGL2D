#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "StudentInfoObject.h"
#include "ClockScreen.h"
#include "EkgObject.h"
#include "BatteryObject.h"

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
    StudentInfoObject* studentInfo;

    void drawArrows(ObjectRenderer& renderer);
    bool checkArrowClick(double xpos, double ypos, bool isLeft);
    void drawWatchBezel(ObjectRenderer& renderer);

public:
    ScreenManager(int screenWidth, int screenHeight, unsigned int rightArrow);
    ~ScreenManager();

    void setClockScreen(ClockScreen* clock) { clockScreen = clock; }
    void setEkgScreen(EkgObject* ekg) { ekgScreen = ekg; }
    void setBatteryScreen(BatteryObject* battery) { batteryScreen = battery; }
	void setStudentInfo(StudentInfoObject* info) { studentInfo = info; }

    void handleClick(double xpos, double ypos);
    void draw(ObjectRenderer& renderer, TextRenderer& textRenderer, bool isRunning);

    Screen getCurrentScreen() const { return currentScreen; }
};
