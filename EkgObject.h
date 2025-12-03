#pragma once

#include <GL/glew.h>
#include <random>

class ObjectRenderer;
class TextRenderer;

class EkgObject {
private:
    unsigned int texture;
    int wWidth, wHeight;
    float scrollOffset;
    float scale;
    int heartRate;
    double lastUpdate;
    bool isRunning;
    unsigned int warningTexture;

    std::mt19937 gen;
    std::uniform_int_distribution<int> heartDist;

public:
    EkgObject(unsigned int ekgTex, unsigned int warnTex, int screenWidth, int screenHeight);
    ~EkgObject();

    void update(double currentTime, bool running);
    void draw(ObjectRenderer& renderer, TextRenderer& textRenderer);

    int getHeartRate() const { return heartRate; }
};