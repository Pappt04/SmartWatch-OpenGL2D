#pragma once
#include <GL/glew.h>
#include "Util.h"
#include <string>

class BatteryObject {
private:
    unsigned int batteryTexture;
    int percent;
    double lastUpdate;
    
    float batteryWidth;
    float batteryHeight;
    float batteryX;
    float batteryY;
    
    int wWidth;
    int wHeight;

public:
    BatteryObject(unsigned texture,int screenWidth, int screenHeight);
    ~BatteryObject();
    
    void update(double currentTime);
    void draw(unsigned int shader, unsigned int VAO);
    void drawColoredRect(unsigned int shader, unsigned int VAO, 
                         float x, float y, float width, float height,
                         float r, float g, float b);
    
    int getPercent() const { return percent; }
    void setPercent(int p) { percent = p; }
};

