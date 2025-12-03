#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class ObjectRenderer {
private:
    unsigned int shader;
    unsigned int VAO;
    unsigned int VBO;
    int wWidth, wHeight;

    void verticesInit();
    void setupProjection();

public:
    ObjectRenderer(unsigned int initShader, int screenWidth, int screenHeight);
    ~ObjectRenderer();

    void Draw(unsigned int texture, const glm::mat3& transform);
    void Draw(unsigned int texture, glm::vec2 position, glm::vec2 size,
        float rotation = 0.0f, float texOffsetX = 0.0f, float texScaleX = 1.0f);

    // Draw with flipping
    void DrawFlipped(unsigned int texture, glm::vec2 position, glm::vec2 size,
        bool flipHorizontal = false, bool flipVertical = false,
        float rotation = 0.0f);

    // For colored rectangles without texture
    void DrawColored(glm::vec2 position, glm::vec2 size,
        glm::vec3 color, float rotation = 0.0f);
};