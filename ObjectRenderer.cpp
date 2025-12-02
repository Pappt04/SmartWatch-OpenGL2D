#include "ObjectRenderer.h"

ObjectRenderer::ObjectRenderer(unsigned initShader): shader(initShader)
{
	ObjectRenderer::verticesInit();
}

ObjectRenderer::~ObjectRenderer()
{
    glDeleteVertexArrays(1, &VAO);
}

void ObjectRenderer::Draw(unsigned texture, const glm::mat3& transform) {
    glm::mat4 model(1.0f);

    model[0][0] = transform[0][0]; // a
    model[0][1] = transform[0][1]; // c
    model[1][0] = transform[1][0]; // b
    model[1][1] = transform[1][1]; // d

    model[3][0] = transform[2].x;
    model[3][1] = transform[2].y;

    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader, "uModel"), 1, GL_FALSE, glm::value_ptr(model));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}


void ObjectRenderer::Draw(unsigned texture, glm::vec2 position, glm::vec2 size, glm::float32 rotation) {
    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));
    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
    model = glm::rotate(model, rotation, glm::vec3(0, 0, 1));
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));
    model = glm::scale(model, glm::vec3(size, 1.0f));

    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader, "uModel"), 1, GL_FALSE, glm::value_ptr(model));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}


void ObjectRenderer::verticesInit()
{
    float verticesRect[] = {
     -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, // upper left vertex
	 -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // bottom left vertex
     0.5f, -0.5f, 1.0f, 0.0f, 0.0f, // bottom right vertex
	 0.5f, 0.5f, 0.0f, 1.0f, 1.0f, // upper right vertex
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesRect), verticesRect, GL_STATIC_DRAW);


    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}
