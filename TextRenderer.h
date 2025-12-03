#pragma once
#include <GL/glew.h>
#include <string>

class TextRenderer {
private:
    unsigned int textShader;
    int wWidth;
    int wHeight;

public:
    TextRenderer(unsigned int shader, int screenWidth, int screenHeight);
    ~TextRenderer();
    
    void renderText(const std::string& text, float x, float y, float scale, 
                    float r, float g, float b);
    void setShader(unsigned int shader) { textShader = shader; }
    void renderTextRectangle(unsigned int shader, std::string text, float x, float y, float width, float height, float r, float g, float b);
};

