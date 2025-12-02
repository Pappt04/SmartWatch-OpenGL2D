#pragma once
#include "GL/glew.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


// Class for rendering rectangle objects
class ObjectRenderer
{
public:
	unsigned shader;
	unsigned VAO;
	unsigned VBO;

	ObjectRenderer(unsigned initShader);
	~ObjectRenderer();

	void Draw(unsigned texture, glm::vec2 position, glm::vec2 size, glm::float32 rotation = 0.0f);
	void Draw(unsigned texture, const glm::mat3& transform);
private:
	void verticesInit();
};

