#include "StudentInfoObject.h"
#include "ObjectRenderer.h"
#include "TextRenderer.h"

extern int wWidth, wHeight;

StudentInfoObject::StudentInfoObject(std::string studentname,std::string studentindex,int screenWidth, int screenHeight)
    : wWidth(screenWidth), wHeight(screenHeight) {
    
	studentName = studentname;
	studentIndex = studentindex;
    cardWidth = 300.0f;
    cardHeight = 120.0f;
    cardX = wWidth - cardWidth - 30.0f;
    cardY = 30.0f;
}

StudentInfoObject::~StudentInfoObject() {
}

void StudentInfoObject::draw(ObjectRenderer& renderer, TextRenderer& textRenderer) {
    glm::vec3 cardColor(0.15f, 0.15f, 0.2f);
    renderer.DrawRectangle(cardX, cardY, cardWidth, cardHeight, cardColor);

    glm::vec3 borderColor(0.3f, 0.3f, 0.4f);
    float borderThickness = 2.0f;
    renderer.DrawRectangle(cardX, cardY + cardHeight - borderThickness, cardWidth, borderThickness, borderColor);
    renderer.DrawRectangle(cardX, cardY, cardWidth, borderThickness, borderColor);
    renderer.DrawRectangle(cardX, cardY, borderThickness, cardHeight, borderColor);
    renderer.DrawRectangle(cardX + cardWidth - borderThickness, cardY, borderThickness, cardHeight, borderColor);

    // Render student name
    float nameTextWidth = 250.0f;
    float nameTextHeight = 40.0f;
    float nameX = cardX + (cardWidth - nameTextWidth) / 2.0f;
    float nameY = cardY + cardHeight - nameTextHeight - 15.0f;
    textRenderer.renderTextRectangle(0, studentName, nameX, nameY, nameTextWidth, nameTextHeight, 1.0f, 1.0f, 1.0f);

    // Render student index
    float indexTextWidth = 200.0f;
    float indexTextHeight = 35.0f;
    float indexX = cardX + (cardWidth - indexTextWidth) / 2.0f;
    float indexY = cardY + 15.0f;
    textRenderer.renderTextRectangle(0, studentIndex, indexX, indexY, indexTextWidth, indexTextHeight, 0.8f, 0.8f, 0.8f);
}
