#version 330 core

in vec2 vUv;

uniform sampler2D uTexture;
uniform vec4 uColor;

out vec4 fragColor;

void main() {
	vec4 texColor = texture(uTexture, vUv);
	if (uColor.a == 0.0) discard;
	fragColor = texColor * uColor;
}







