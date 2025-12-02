#pragma once
#include "GL/glew.h"

#include <glm/glm.hpp>

// Class for rendering rectangle objects
class ObjectRenderer
{
public:
	GLuint shader;
	GLuint VAO;
	GLuint VBO;

	ObjectRenderer(GLuint initShader);
	~ObjectRenderer();

	void Draw(GLuint texture, glm::vec2 position);
private:
	void verticesInit();
};

