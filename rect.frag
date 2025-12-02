#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
uniform sampler2D uTexture;
uniform vec3 uColor;
uniform int uUseTexture;

void main()
{
    if (uUseTexture == 1) {
        FragColor = texture(uTexture, TexCoords);
    } else {
        FragColor = vec4(uColor, 1.0);
    }
}