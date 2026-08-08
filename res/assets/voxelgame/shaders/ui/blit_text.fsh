#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D uTexture;
uniform vec4 uColor;

void main() {
    float alpha = texture(uTexture, TexCoords).r;
    FragColor = vec4(uColor.rgb, uColor.a * alpha);
}