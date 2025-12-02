#pragma once
#include <GL/glew.h>
#include <random>

class EkgObject {
private:
    unsigned int texture;
    float scrollOffset;
    float scale;
    int heartRate;
    double lastUpdate;
    bool isRunning;
    
    int wWidth;
    int wHeight;
    
    std::mt19937 gen;
    std::uniform_int_distribution<> heartDist;

public:
    EkgObject(unsigned tex,int screenWidth, int screenHeight);
    ~EkgObject();
    
    void update(double currentTime, bool running);
    void draw(unsigned int shader, unsigned int VAO);
    
    int getHeartRate() const { return heartRate; }
    void setHeartRate(int rate) { heartRate = rate; }
};

