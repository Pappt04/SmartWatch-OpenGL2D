#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <ft2build.h>
#include <map>
#include FT_FREETYPE_H

//Char
struct Character {
    unsigned int TextureID;
    int SizeX;
    int SizeY;        
    int BearingX;    
    int BearingY;
    unsigned int Advance;
};

extern std::map<char, Character> Characters;

unsigned int createShader(const char* vsSource, const char* fsSource);
unsigned loadImageToTexture(const char* filePath);
GLFWcursor* loadImageToCursor(const char* filePath);

bool initFreeType(const char* fontPath, unsigned int fontSize);
void cleanupFreeType();
void renderText(unsigned int shader, std::string text, float x, float y, float scale, float r, float g, float b);
