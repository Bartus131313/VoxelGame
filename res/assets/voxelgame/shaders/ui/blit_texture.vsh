#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aUV;

uniform mat4 uProjection;  // pixel-space orthographic projection
uniform vec2 uPosition;    // top-left screen position in pixels
uniform vec2 uSize;        // width/height in pixels
uniform vec4 uUVBox;       // (uMin, vMin, uMax, vMax) sub-rect inside the atlas

out vec2 vUV;

void main() {
    // Scale the unit quad to the requested pixel size and move it to (uPosition).
    vec2 screenPos = uPosition + aPos * uSize;

    // Remap the quad's local 0..1 uv into the atlas sub-rect for this texture.
    vUV = mix(uUVBox.xy, uUVBox.zw, aUV);

    gl_Position = uProjection * vec4(screenPos, 0.0, 1.0);
}
