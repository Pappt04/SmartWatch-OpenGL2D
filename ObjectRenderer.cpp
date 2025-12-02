#include "ObjectRenderer.h"

ObjectRenderer::ObjectRenderer(GLuint initShader): shader(initShader)
{
	ObjectRenderer::verticesInit();
}

ObjectRenderer::~ObjectRenderer()
{
    glDeleteVertexArrays(1, &VAO);
}

void ObjectRenderer::Draw(GLuint texture, glm::vec2 position, glm::vec2 size, glm::float32 rotation)
{

}


void ObjectRenderer::verticesInit()
{
    float verticesRect[] = {
     -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, // gornje levo teme
     -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // donje levo teme
     0.5f, -0.5f, 1.0f, 0.0f, 0.0f, // donje desno teme
     0.5f, 0.5f, 0.0f, 1.0f, 1.0f, // gornje desno teme
    };

}
