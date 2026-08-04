#version 330 core

out vec4 FragColor;

in vec3 Direction;

uniform vec3 zenithDayColor;
uniform vec3 horizonDayColor;
uniform vec3 zenithNightColor;
uniform vec3 horizonNightColor;
uniform float daylightFactor; // 0 = full night, 1 = full day
uniform vec3 sunDirection;    // Normalized world-space direction toward the sun
uniform float time;           // Seconds elapsed, used to gently twinkle the stars

// Cheap hash-based pseudo-random value in [0, 1] for a 3D input
float hash3(vec3 p) {
    p = fract(p * 0.3183099 + vec3(0.1, 0.2, 0.3));
    p *= 17.0;
    return fract(p.x * p.y * p.z * (p.x + p.y + p.z));
}

// Projects a direction onto one of 6 cube faces and returns stable 2D face-local coordinates.
// Using true face UVs (rather than raw 3D grid coords) keeps star shapes undistorted.
vec2 getFaceUV(vec3 dir, out float faceId) {
    vec3 ad = abs(dir);
    if (ad.x >= ad.y && ad.x >= ad.z) {
        faceId = dir.x > 0.0 ? 0.0 : 1.0;
        return vec2(dir.z, dir.y) / ad.x;
    } else if (ad.y >= ad.z) {
        faceId = dir.y > 0.0 ? 2.0 : 3.0;
        return vec2(dir.x, dir.z) / ad.y;
    } else {
        faceId = dir.z > 0.0 ? 4.0 : 5.0;
        return vec2(dir.x, dir.y) / ad.z;
    }
}

// Sparse field of small, twinkling stars, projected onto stable cube-face UVs.
// Searches the current cell plus its 3x3 neighborhood so a star whose jittered position sits
// near a cell edge still renders its full circle/spike instead of getting clipped at the border.
float stars(vec3 dir) {
    float faceId;
    vec2 uv = getFaceUV(dir, faceId);

    const float density = 16.0;
    vec2 guv = uv * density;
    vec2 baseCell = floor(guv);
    vec2 fracPos = fract(guv);

    float star = 0.0;

    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            vec2 cellOffset = vec2(float(dx), float(dy));
            vec2 cell = baseCell + cellOffset;

            float starChance = hash3(vec3(cell, faceId));
            if (starChance < 0.975) continue;

            vec2 jitter = vec2(hash3(vec3(cell + 11.1, faceId)), hash3(vec3(cell + 23.7, faceId)));
            // Position of the current fragment relative to this neighbor's star center
            vec2 localPos = fracPos - cellOffset - jitter;
            float dist = length(localPos);

            float brightnessSeed = hash3(vec3(cell + 91.7, faceId));

            // A minority of stars are bigger "hero" sparkles with a 4-point glint;
            // the rest stay as small simple points.
            bool hero = brightnessSeed > 0.8;
            float coreRadius = hero ? 0.16 : 0.08;
            float core = smoothstep(coreRadius, 0.0, dist);

            float contribution = core;
            if (hero) {
                float rotSeed = hash3(vec3(cell + 63.4, faceId));
                float angle = rotSeed * 6.2831853;
                float c = cos(angle), s = sin(angle);
                vec2 p = mat2(c, -s, s, c) * localPos;

                float spikeLen = 0.45;
                float spikeWidth = 0.045;
                float spikeX = (1.0 - smoothstep(0.0, spikeWidth, abs(p.y))) * max(0.0, 1.0 - abs(p.x) / spikeLen);
                float spikeY = (1.0 - smoothstep(0.0, spikeWidth, abs(p.x))) * max(0.0, 1.0 - abs(p.y) / spikeLen);
                float spike = max(spikeX, spikeY);
                contribution = max(core, spike * 0.85);
            }

            float twinkle = 0.65 + 0.35 * sin(time * (1.5 + brightnessSeed * 2.5) + brightnessSeed * 6.2831);
            contribution *= (0.4 + 0.6 * brightnessSeed) * twinkle;

            star = max(star, contribution);
        }
    }

    return star;
}

void main() {
    vec3 dir = normalize(Direction);

    // Blend zenith/horizon colors between day and night palettes based on daylight
    vec3 zenithColor = mix(zenithNightColor, zenithDayColor, daylightFactor);
    vec3 horizonColor = mix(horizonNightColor, horizonDayColor, daylightFactor);

    // Vertical gradient factor: 0 at horizon, 1 straight up. Biased with pow() so the
    // horizon band stays fairly thin, matching Minecraft's look.
    float heightFactor = clamp(dir.y, 0.0, 1.0);
    heightFactor = pow(heightFactor, 0.45);

    vec3 skyColor = mix(horizonColor, zenithColor, heightFactor);

    // Warm sunrise/sunset glow near the horizon when the sun itself is near the horizon
    float sunHeight = sunDirection.y;
    float sunsetStrength = 1.0 - smoothstep(0.0, 0.35, abs(sunHeight));
    float horizonProximity = 1.0 - smoothstep(0.0, 0.4, abs(dir.y));
    float facingSun = clamp(dot(dir, vec3(sunDirection.x, 0.0, sunDirection.z)) /
                             max(length(vec2(sunDirection.x, sunDirection.z)), 0.0001), 0.0, 1.0);
    vec3 sunsetColor = vec3(1.0, 0.55, 0.25);
    skyColor = mix(skyColor, sunsetColor, sunsetStrength * horizonProximity * facingSun * 0.55);

    // Sky below the horizon fades toward the horizon color rather than showing "empty" void color
    if (dir.y < 0.0) {
        skyColor = mix(skyColor, horizonColor, clamp(-dir.y * 2.0, 0.0, 1.0));
    }

    // Stars are only visible at night, faded out near the horizon and during daylight/twilight,
    // matching Minecraft's behavior of stars only showing well above the true night sky.
    float nightAmount = 1.0 - smoothstep(0.0, 0.35, daylightFactor);
    float starHorizonFade = smoothstep(0.02, 0.25, dir.y);
    float starVisibility = nightAmount * starHorizonFade;

    if (starVisibility > 0.0) {
        float s = stars(dir) * starVisibility;
        skyColor += vec3(s);
    }

    FragColor = vec4(skyColor, 1.0);
}
