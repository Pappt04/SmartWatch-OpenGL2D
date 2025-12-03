#include "EkgObject.h"
#include "ObjectRenderer.h"
#include "TextRenderer.h"
#include <algorithm>
#include <string>

extern int wWidth, wHeight;

EkgObject::EkgObject(unsigned int ekgTex, unsigned int warnTex, int screenWidth, int screenHeight)
    : texture(ekgTex), warningTexture(warnTex), wWidth(screenWidth), wHeight(screenHeight),
    scrollOffset(0.0f), scale(1.0f), heartRate(70), lastUpdate(0.0), isRunning(false),
    heartDist(60, 80) {
    std::random_device rd;
    gen.seed(rd());
}

EkgObject::~EkgObject() {
    if (texture != 0) {
        glDeleteTextures(1, &texture);
    }
    if (warningTexture != 0) {
        glDeleteTextures(1, &warningTexture);
    }
}

void EkgObject::update(double currentTime, bool running) {
    isRunning = running;

    if (currentTime - lastUpdate >= 0.1) {
        lastUpdate = currentTime;

        if (isRunning) {
            heartRate = std::min(220, heartRate + 2);
        }
        else {
            if (heartRate > 70) {
                heartRate = std::max(70, heartRate - 1);
            }
            else {
                heartRate = heartDist(gen);
            }
        }
    }

    // Scrolling speed increases with heart rate
    scrollOffset += 0.0005f * (heartRate / 70.0f);
    if (scrollOffset > 1.0f) scrollOffset -= 1.0f;
}

void EkgObject::draw(ObjectRenderer& renderer, TextRenderer& textRenderer) {
    if (texture == 0) return;

    // Calculate height based on BPM - scaled to fit within watch
    // Higher BPM = TALLER waves (more energetic heart)
    // Lower BPM = SHORTER waves (calm heart)
    float baseHeight = 150.0f;  // Reduced to fit in watch
    float maxHeight = 250.0f;   // Reduced to fit in watch
    float heightScale = 1.0f + ((heartRate - 70.0f) / 150.0f);
    heightScale = std::max(1.0f, std::min(1.67f, heightScale));
    float ekgHeight = baseHeight * heightScale;

    // Width scaled to fit within the watch (leave room for arrows)
    float ekgWidth = 400.0f;  // Reduced to fit better in watch

    // Center the EKG within the watch
    float centerX = wWidth / 2.0f;
    float centerY = wHeight / 2.0f;
    
    glm::vec2 ekgSize(ekgWidth, ekgHeight);
	glm::uvec2 ekgPos(centerX, centerY);  

    // Frequency increases with heart rate
    float texScale = 2.f + ((heartRate - 70.0f) / 150.0f) * 1.2f;
    texScale = std::max(0.3f, std::min(1.5f, texScale));

    // Pass texture offset and scale to create scrolling effect
    renderer.Draw(texture, ekgPos, ekgSize, 0.0f, scrollOffset, texScale);
	
    // Draw heart rate text using renderTextRectangle - positioned below EKG
    std::string bpmText = std::to_string(heartRate) + " BPM";
    float textRectWidth = 200.0f;
    float textRectHeight = 60.0f;
    float textX = centerX - textRectWidth / 2.0f;
    float textY = centerY + ekgHeight / 2.0f + 20.0f;  // Below the EKG
    textRenderer.renderTextRectangle(0, bpmText, textX, textY, textRectWidth, textRectHeight, 1.0f, 0.3f, 0.3f);

    // Draw warning if heart rate > 200 - scaled to fit in watch
    if (heartRate > 200) {
        float warnSize = 300.0f;  // Reduced from 600
        glm::vec2 warnPos(centerX - warnSize / 2.0f, centerY - warnSize / 2.0f);
        glm::vec2 warnSizeVec(warnSize, warnSize);
        
        //renderer.Draw(warningTexture, warnPos, warnSizeVec, 0.0f);
		renderer.DrawTexturedRectangle(warningTexture, warnPos.x, warnPos.y, warnSizeVec.x, warnSizeVec.y);

        float warnTextWidth = 350.0f;
        float warnTextHeight = 60.0f;
        float warnTextX = centerX - warnTextWidth / 2.0f;
        float warnTextY = centerY - 30.0f;
        textRenderer.renderTextRectangle(0, "STOP! ODMORI SE!", warnTextX, warnTextY, warnTextWidth, warnTextHeight, 1.0f, 0.0f, 0.0f);
    }
}