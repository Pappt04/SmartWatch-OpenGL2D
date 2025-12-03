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
    scrollOffset += 0.0001f * (heartRate / 70.0f);
    if (scrollOffset > 1.0f) scrollOffset -= 1.0f;
}

void EkgObject::draw(ObjectRenderer& renderer, TextRenderer& textRenderer) {
    if (texture == 0) return;

    // Calculate height based on BPM
    // Higher BPM = TALLER waves (more energetic heart)
    // Lower BPM = SHORTER waves (calm heart)
    // Range: 200px at 70 BPM up to 400px at 220 BPM
    float baseHeight = 200.0f;
    float maxHeight = 400.0f;
    float heightScale = 1.0f + ((heartRate - 70.0f) / 150.0f);  // 1.0 to 2.0
    heightScale = std::max(1.0f, std::min(2.0f, heightScale));
    float ekgHeight = baseHeight * heightScale;

    // Width stays constant for better visibility
    float ekgWidth = 800.0f;

    glm::vec2 ekgPos(wWidth / 2.0f - ekgWidth / 2.0f, wHeight / 2.0f - ekgHeight / 2.0f);
    glm::vec2 ekgSize(ekgWidth, ekgHeight);

    // Frequency increases with heart rate
    // Higher BPM = MORE waves (higher frequency)
    // Lower BPM = FEWER waves (lower frequency)
    // Scale range: 0.3 at rest to 1.5 when running
    float texScale = 0.3f + ((heartRate - 70.0f) / 150.0f) * 1.2f;  // 0.3 to 1.5
    texScale = std::max(0.3f, std::min(1.5f, texScale));

    // Pass texture offset and scale to create scrolling effect
    renderer.Draw(texture, ekgPos, ekgSize, 0.0f, scrollOffset, texScale);

    // Draw heart rate text
    std::string bpmText = std::to_string(heartRate) + " BPM";
    float textX = wWidth / 2.0f - 120.0f;
    float textY = wHeight / 2.0f + 250.0f;
    textRenderer.renderText(bpmText, textX, textY, 1.2f, 1.0f, 0.3f, 0.3f);

    // Draw warning if heart rate > 200
    if (heartRate > 200) {
        glm::vec2 warnPos(wWidth / 2.0f - 300.0f, wHeight / 2.0f - 300.0f);
        glm::vec2 warnSize(600.0f, 600.0f);
        renderer.Draw(warningTexture, warnPos, warnSize, 0.0f);

        textRenderer.renderText("STOP! ODMORI SE!",
            wWidth / 2.0f - 250.0f, wHeight / 2.0f - 50.0f,
            1.2f, 1.0f, 0.0f, 0.0f);
    }
}