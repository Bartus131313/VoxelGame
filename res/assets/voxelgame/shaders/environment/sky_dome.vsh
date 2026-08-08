#version 330 core

layout (location = 0) in vec3 aPos;

out vec3 Direction; // World-space direction from camera, used for gradient lookup

flat out vec3 vZenithColor;
flat out vec3 vHorizonColor;
flat out float vSunsetStrength;
flat out float vNightAmount;
flat out vec2 vSunDirHorizNorm;

uniform mat4 uProjection;
uniform mat4 uView;

uniform vec3 uZenithDayColor;
uniform vec3 uHorizonDayColor;
uniform vec3 uZenithNightColor;
uniform vec3 uHorizonNightColor;
uniform float uDaylightFactor;
uniform vec3 uSunDirection;

void main() {
    Direction = aPos;

    vZenithColor = mix(uZenithNightColor, uZenithDayColor, uDaylightFactor);
    vHorizonColor = mix(uHorizonNightColor, uHorizonDayColor, uDaylightFactor);

    float sunHeight = uSunDirection.y;
    vSunsetStrength = 1.0 - smoothstep(0.0, 0.35, abs(sunHeight));
    vNightAmount = 1.0 - smoothstep(0.0, 0.35, uDaylightFactor);

    vec2 sunHoriz = vec2(uSunDirection.x, uSunDirection.z);
    vSunDirHorizNorm = sunHoriz / max(length(sunHoriz), 0.0001);

    // Strip translation so the dome stays centered on the camera, same trick as a cubemap skybox
    mat4 staticView = mat4(mat3(uView));
    vec4 pos = uProjection * staticView * vec4(aPos, 1.0);

    // Push to far plane
    gl_Position = pos.xyww;
}
