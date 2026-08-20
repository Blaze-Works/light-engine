#version 330 core

in vec4 vColor;

uniform vec4 uColor;

out vec4 FragColor;

void main() {
	if (uColor.a == 0.0) discard;
	FragColor = uColor;
}