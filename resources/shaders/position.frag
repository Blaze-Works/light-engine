#version 330 core
out vec4 FragColor;
uniform vec4 uColor;

void main() {
    if (uColor.a == 0.0) discard;
    FragColor = uColor;
}
