#version 330 core

layout (location = 0) in vec3 aPos;

out vec3 Direction; // World-space direction from camera, used for gradient lookup

flat out vec3 vZenithColor;
flat out vec3 vHorizonColor;
flat out float vSunsetStrength;
flat out float vNightAmount;
flat out vec2 vSunDirHorizNorm;

uniform mat4 projection;
uniform mat4 view;

uniform vec3 zenithDayColor;
uniform vec3 horizonDayColor;
uniform vec3 zenithNightColor;
uniform vec3 horizonNightColor;
uniform float daylightFactor;
uniform vec3 sunDirection;

void main() {
    Direction = aPos;

    vZenithColor = mix(zenithNightColor, zenithDayColor, daylightFactor);
    vHorizonColor = mix(horizonNightColor, horizonDayColor, daylightFactor);

    float sunHeight = sunDirection.y;
    vSunsetStrength = 1.0 - smoothstep(0.0, 0.35, abs(sunHeight));
    vNightAmount = 1.0 - smoothstep(0.0, 0.35, daylightFactor);

    vec2 sunHoriz = vec2(sunDirection.x, sunDirection.z);
    vSunDirHorizNorm = sunHoriz / max(length(sunHoriz), 0.0001);

    // Strip translation so the dome stays centered on the camera, same trick as a cubemap skybox
    mat4 staticView = mat4(mat3(view));
    vec4 pos = projection * staticView * vec4(aPos, 1.0);

    // Push to far plane
    gl_Position = pos.xyww;
}
