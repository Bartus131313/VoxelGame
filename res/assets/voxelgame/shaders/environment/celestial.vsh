#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 uProjection;
uniform mat4 uView;
uniform vec3 uWorldPosition;    // Position of the quad center, far along the sun/moon direction
uniform vec3 uRightVector;      // Billboard right vector
uniform vec3 uUpVector;         // Billboard up vector
uniform float uSize;

void main() {
    TexCoord = aTexCoord;

    vec3 vertexWorldPos = uWorldPosition + (uRightVector * aPos.x + uUpVector * aPos.y) * uSize;

    mat4 staticView = mat4(mat3(uView));
    vec4 pos = uProjection * staticView * vec4(vertexWorldPos, 1.0);

    gl_Position = vec4(pos.x, pos.y, pos.w * 0.999999, pos.w);
}
