#version 330 core

layout (location = 0) in vec3 aPos;

out vec3 Direction; // World-space direction from camera, used for gradient lookup

uniform mat4 projection;
uniform mat4 view;

void main() {
    Direction = aPos;

    // Strip translation so the dome stays centered on the camera, same trick as a cubemap skybox
    mat4 staticView = mat4(mat3(view));
    vec4 pos = projection * staticView * vec4(aPos, 1.0);

    // Push to far plane
    gl_Position = pos.xyww;
}
