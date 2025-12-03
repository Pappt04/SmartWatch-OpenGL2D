#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D uTexture;
uniform vec3 uColor;
uniform int uUseTexture;
uniform float uTexOffsetX;
uniform float uTexScaleX;

void main() {
    if (uUseTexture == 1) {
        // Apply texture scrolling and scaling for EKG animation
        vec2 scrolledCoord = vec2(TexCoord.x * uTexScaleX + uTexOffsetX, TexCoord.y);
        FragColor = texture(uTexture, scrolledCoord);
    } else {
        // Solid color rendering for battery fill
        FragColor = vec4(uColor, 1.0);
    }
}