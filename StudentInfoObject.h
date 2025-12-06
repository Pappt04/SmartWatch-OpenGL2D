#pragma once

#include <GL/glew.h>
#include <string>

class ObjectRenderer;
class TextRenderer;

class StudentInfoObject {
private:
    int wWidth, wHeight;
    float cardWidth;
    float cardHeight;
    float cardX;
    float cardY;
	std::string studentName;
	std::string studentIndex;

public:
    StudentInfoObject(std::string studentname, std::string studentindex,int screenWidth, int screenHeight);
    ~StudentInfoObject();

    void draw(ObjectRenderer& renderer, TextRenderer& textRenderer);
};
