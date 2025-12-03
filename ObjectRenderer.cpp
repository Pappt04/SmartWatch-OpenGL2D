#include "ObjectRenderer.h"

ObjectRenderer::ObjectRenderer(unsigned int initShader, int screenWidth, int screenHeight)
    : shader(initShader), wWidth(screenWidth), wHeight(screenHeight) {
    verticesInit();
}

ObjectRenderer::~ObjectRenderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void ObjectRenderer::setupProjection() {
    float left = 0.0f;
    float right = (float)wWidth;
    float bottom = 0.0f;
    float top = (float)wHeight;

    glm::mat4 projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);

    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader, "uProjection"),
        1, GL_FALSE, glm::value_ptr(projection));
}

void ObjectRenderer::Draw(unsigned int texture, const glm::mat3& transform) {
    glm::mat4 model(1.0f);

    model[0][0] = transform[0][0];
    model[0][1] = transform[0][1];
    model[1][0] = transform[1][0];
    model[1][1] = transform[1][1];
    model[3][0] = transform[2].x;
    model[3][1] = transform[2].y;

    glUseProgram(shader);
    setupProjection();
    glUniformMatrix4fv(glGetUniformLocation(shader, "uModel"),
        1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(glGetUniformLocation(shader, "uUseTexture"), 1);
    glUniform1f(glGetUniformLocation(shader, "uTexOffsetX"), 0.0f);
    glUniform1f(glGetUniformLocation(shader, "uTexScaleX"), 1.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
}

void ObjectRenderer::Draw(unsigned int texture, glm::vec2 position, glm::vec2 size,
    float rotation, float texOffsetX, float texScaleX) {
    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));
    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
    model = glm::rotate(model, rotation, glm::vec3(0, 0, 1));
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));
    model = glm::scale(model, glm::vec3(size, 1.0f));

    glUseProgram(shader);
    setupProjection();
    glUniformMatrix4fv(glGetUniformLocation(shader, "uModel"),
        1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(glGetUniformLocation(shader, "uUseTexture"), 1);
    glUniform1f(glGetUniformLocation(shader, "uTexOffsetX"), texOffsetX);
    glUniform1f(glGetUniformLocation(shader, "uTexScaleX"), texScaleX);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
}

void ObjectRenderer::DrawFlipped(unsigned int texture, glm::vec2 position, glm::vec2 size,
    bool flipHorizontal, bool flipVertical, float rotation) {
    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));
    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
    model = glm::rotate(model, rotation, glm::vec3(0, 0, 1));

    // Apply flip by scaling negatively
    float scaleX = flipHorizontal ? -size.x : size.x;
    float scaleY = flipVertical ? -size.y : size.y;
    model = glm::scale(model, glm::vec3(scaleX, scaleY, 1.0f));
    model = glm::translate(model, glm::vec3(-0.5f, -0.5f, 0.0f));

    glUseProgram(shader);
    setupProjection();
    glUniformMatrix4fv(glGetUniformLocation(shader, "uModel"),
        1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(glGetUniformLocation(shader, "uUseTexture"), 1);
    glUniform1f(glGetUniformLocation(shader, "uTexOffsetX"), 0.0f);
    glUniform1f(glGetUniformLocation(shader, "uTexScaleX"), 1.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
}

void ObjectRenderer::DrawColored(glm::vec2 position, glm::vec2 size,
    glm::vec3 color, float rotation) {
    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));
    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
    model = glm::rotate(model, rotation, glm::vec3(0, 0, 1));
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));
    model = glm::scale(model, glm::vec3(size, 1.0f));

    glUseProgram(shader);
    setupProjection();
    glUniformMatrix4fv(glGetUniformLocation(shader, "uModel"),
        1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(glGetUniformLocation(shader, "uUseTexture"), 0);
    glUniform3f(glGetUniformLocation(shader, "uColor"), color.r, color.g, color.b);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
}

void ObjectRenderer::DrawRectangle(float x, float y, float width, float height, glm::vec3 color) {
    // Use vertices anchored at origin (0,0) instead of centered vertices
    float vertices[] = {
        0.0f, 0.0f, 0.0f, 0.0f,  // bottom left
        1.0f, 0.0f, 1.0f, 0.0f,  // bottom right
        1.0f, 1.0f, 1.0f, 1.0f,  // top right
        0.0f, 1.0f, 0.0f, 1.0f   // top left
    };

    // Create temporary VAO and VBO for origin-based rectangle
    unsigned int tempVAO, tempVBO;
    glGenVertexArrays(1, &tempVAO);
    glGenBuffers(1, &tempVBO);

    glBindVertexArray(tempVAO);
    glBindBuffer(GL_ARRAY_BUFFER, tempVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
        (void*)(2 * sizeof(float)));

    // Apply transformations
    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(x, y, 0.0f));
    model = glm::scale(model, glm::vec3(width, height, 1.0f));

    glUseProgram(shader);
    setupProjection();
    glUniformMatrix4fv(glGetUniformLocation(shader, "uModel"),
        1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(glGetUniformLocation(shader, "uUseTexture"), 0);
    glUniform3f(glGetUniformLocation(shader, "uColor"), color.r, color.g, color.b);

    glBindVertexArray(tempVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // Cleanup
    glBindVertexArray(0);
    glDeleteBuffers(1, &tempVBO);
    glDeleteVertexArrays(1, &tempVAO);
}

void ObjectRenderer::DrawTexturedRectangle(unsigned int texture, float x, float y,
    float width, float height) {
    // Use vertices anchored at origin (0,0) instead of centered vertices
    float vertices[] = {
        0.0f, 0.0f, 0.0f, 0.0f,  // bottom left
        1.0f, 0.0f, 1.0f, 0.0f,  // bottom right
        1.0f, 1.0f, 1.0f, 1.0f,  // top right
        0.0f, 1.0f, 0.0f, 1.0f   // top left
    };

    // Create temporary VAO and VBO for origin-based rectangle
    unsigned int tempVAO, tempVBO;
    glGenVertexArrays(1, &tempVAO);
    glGenBuffers(1, &tempVBO);

    glBindVertexArray(tempVAO);
    glBindBuffer(GL_ARRAY_BUFFER, tempVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
        (void*)(2 * sizeof(float)));

    // Apply transformations
    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(x, y, 0.0f));
    model = glm::scale(model, glm::vec3(width, height, 1.0f));

    glUseProgram(shader);
    setupProjection();
    glUniformMatrix4fv(glGetUniformLocation(shader, "uModel"),
        1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(glGetUniformLocation(shader, "uUseTexture"), 1);
    glUniform1f(glGetUniformLocation(shader, "uTexOffsetX"), 0.0f);
    glUniform1f(glGetUniformLocation(shader, "uTexScaleX"), 1.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glBindVertexArray(tempVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // Cleanup
    glBindVertexArray(0);
    glDeleteBuffers(1, &tempVBO);
    glDeleteVertexArrays(1, &tempVAO);
}

void ObjectRenderer::verticesInit() {
    float verticesRect[] = {
        -0.5f, -0.5f, 0.0f, 0.0f,  // bottom left
         0.5f, -0.5f, 1.0f, 0.0f,  // bottom right
         0.5f,  0.5f, 1.0f, 1.0f,  // top right
        -0.5f,  0.5f, 0.0f, 1.0f   // top left
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesRect), verticesRect, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
        (void*)(2 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}