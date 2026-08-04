#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 projection;
uniform mat4 view;
uniform vec3 worldPosition; // Position of the quad center, far along the sun/moon direction
uniform vec3 right;         // Billboard right vector
uniform vec3 up;            // Billboard up vector
uniform float size;

void main() {
    TexCoord = aTexCoord;

    vec3 vertexWorldPos = worldPosition + (right * aPos.x + up * aPos.y) * size;

    mat4 staticView = mat4(mat3(view));
    vec4 pos = projection * staticView * vec4(vertexWorldPos, 1.0);

    gl_Position = vec4(pos.x, pos.y, pos.w * 0.999999, pos.w);
}
