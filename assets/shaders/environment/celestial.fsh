#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D bodyTexture;
uniform vec2 uvMin; // Atlas rect min
uniform vec2 uvMax; // Atlas rect max
uniform float brightness;

void main() {
    vec2 atlasUV = mix(uvMin, uvMax, TexCoord);
    vec4 texColor = texture(bodyTexture, atlasUV);

    float alpha = texColor.a * brightness;

//    if (alpha < 0.02) discard;

    FragColor = vec4(texColor.rgb, alpha);
}
