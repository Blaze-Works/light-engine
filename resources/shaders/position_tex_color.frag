#version 330 core

in vec2 vUv;
in vec4 vertexColor;

uniform sampler2D uTexture;
uniform vec4 uColor;

out vec4 fragColor;

void main() {
    vec4 texColor = texture(uTexture, vUv) * vertexColor;
    if (uColor.a == 0) discard;
    fragColor = texColor * uColor;
}