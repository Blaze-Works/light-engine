#version 330 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUv;
layout(location = 2) in vec4 uColor;

uniform mat4 uProjection;

out vec2 vUv;
out vec4 vertexColor;

void main() {
    vUv = aUv;
    gl_Position = uProjection * vec4(aPos, 0.0, 1.0);
    vertexColor = uColor;
}