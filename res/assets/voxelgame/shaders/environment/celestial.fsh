#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D uTexture;
uniform vec2 uUVMin;
uniform vec2 uUVMax;
uniform float uBrightness;

void main() {
    vec2 atlasUV = mix(uUVMin, uUVMax, TexCoord);
    vec4 texColor = texture(uTexture, atlasUV);

    float alpha = texColor.a * uBrightness;

    if (alpha < 0.02) discard;

    FragColor = vec4(texColor.rgb, alpha);
}
