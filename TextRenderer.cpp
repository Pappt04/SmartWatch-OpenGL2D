#include "TextRenderer.h"
#include "Util.h"

TextRenderer::TextRenderer(unsigned int shader, int screenWidth, int screenHeight)
    : textShader(shader), wWidth(screenWidth), wHeight(screenHeight) {
}

TextRenderer::~TextRenderer() {
}

void TextRenderer::renderText(const std::string& text, float x, float y, float scale,
                               float r, float g, float b) {
    // Use the external renderText function from Util
    ::renderText(textShader, text, x, y, scale, r, g, b);
}
