#version 330 core

in vec3 vNormal;
in vec2 vUv;
in vec3 vWorldPos;

uniform vec4 uColor;
uniform sampler2D uTexture;
uniform bool uHasTexture;
uniform vec3 uLightDir;

out vec4 FragColor;

void main() {
    vec3 N = normalize(vNormal);
    vec3 L = normalize(-uLightDir);

    float diff = max(dot(N, L), 0.0);
    float ambient = 0.25;
    float lighting = ambient + diff * 0.75;
    vec4 base = uColor;

    if (uHasTexture) {
        base *= texture(uTexture, vUv);
    }

    FragColor = vec4(base.rgb * lighting, base.a);
    if (FragColor.a < 0.01) discard;
}