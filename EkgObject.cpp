#include "EkgObject.h"
#include "Util.h"
#include <algorithm>

extern int wWidth, wHeight;

EkgObject::EkgObject(unsigned tex,int screenWidth, int screenHeight): heartDist(60, 80) {
    std::random_device rd;
    gen.seed(rd());
	texture = tex;
	wWidth = screenWidth;
	wHeight = screenHeight;
	scrollOffset = 0.0f;
	scale = 1.0f;
	heartRate = 70;
	lastUpdate = 0.0;
	isRunning = false;
}

EkgObject::~EkgObject() {
    if (texture != 0) {
        glDeleteTextures(1, &texture);
    }
}


void EkgObject::update(double currentTime, bool running) {
    isRunning = running;
    
    // Update heart rate
    if (currentTime - lastUpdate >= 0.1) {
        lastUpdate = currentTime;
        
        if (isRunning) {
            heartRate = std::min(220, heartRate + 2);
            scale = std::max(0.3f, scale - 0.02f);
        } else {
            if (heartRate > 70) {
                heartRate = std::max(70, heartRate - 1);
            } else {
                heartRate = heartDist(gen);
            }
            scale = std::min(1.0f, scale + 0.01f);
        }
    }
    
    // Update scroll
    scrollOffset += 0.01f * (heartRate / 70.0f);
    if (scrollOffset > 1.0f) scrollOffset -= 1.0f;
}

void EkgObject::draw(unsigned int shader, unsigned int VAO) {
    if (texture == 0) return;
    
    glUseProgram(shader);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    // Projection matrix
    float left = 0.0f;
    float right = (float)wWidth;
    float bottom = 0.0f;
    float top = (float)wHeight;
    
    float projection[16] = {
        2.0f / (right - left), 0, 0, 0,
        0, 2.0f / (top - bottom), 0, 0,
        0, 0, -1, 0,
        -(right + left) / (right - left), -(top + bottom) / (top - bottom), 0, 1
    };
    
    // EKG dimensions and position
    float ekgWidth = 600.0f * scale;
    float ekgHeight = 150.0f;
    float ekgX = wWidth / 2.0f - ekgWidth / 2.0f;
    float ekgY = wHeight / 2.0f - ekgHeight / 2.0f;
    
    // Model matrix
    float model[16] = {
        ekgWidth, 0, 0, 0,
        0, ekgHeight, 0, 0,
        0, 0, 1, 0,
        ekgX, ekgY, 0, 1
    };
    
    glUniformMatrix4fv(glGetUniformLocation(shader, "uProjection"), 1, GL_FALSE, projection);
    glUniformMatrix4fv(glGetUniformLocation(shader, "uModel"), 1, GL_FALSE, model);
    glUniform1f(glGetUniformLocation(shader, "uTexOffsetX"), scrollOffset);
    glUniform1f(glGetUniformLocation(shader, "uTexScaleX"), scale);
    glUniform1i(glGetUniformLocation(shader, "uUseTexture"), 1);
    
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}
