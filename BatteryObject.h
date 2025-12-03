#pragma once

#include <GL/glew.h>

class ObjectRenderer;
class TextRenderer;

class BatteryObject {
private:
    unsigned int batteryTexture;
    int wWidth, wHeight;
    int percent;
    double lastUpdate;
    float batteryHeight;
    float batteryWidth;
    float batteryX;
    float batteryY;

    void drawColoredRect(ObjectRenderer& renderer, float x, float y,
        float width, float height, float r, float g, float b);

public:
    BatteryObject(unsigned int texture, int screenWidth, int screenHeight);
    ~BatteryObject();

    void update(double currentTime);
    void draw(ObjectRenderer& renderer, TextRenderer& textRenderer);

    // Draw battery indicator in top-right corner (for all screens)
    void drawIndicator(ObjectRenderer& renderer, TextRenderer& textRenderer);

    int getPercent() const { return percent; }
};