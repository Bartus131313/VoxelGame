#version 330 core

in vec2 vUV;
out vec4 FragColor;

uniform sampler2D uTexture;
uniform vec3 uColor;

void main() {
    vec4 texColor = texture(uTexture, vUV);
    if (texColor.a <= 0.001) discard;

    FragColor = vec4(texColor.rgb * uColor, texColor.a);
}
