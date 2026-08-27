#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUv;
layout(location = 3) in vec4 aJoints;
layout(location = 4) in vec4 aWeights;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat3 uNormalMatrix;
uniform bool uUseSkinning;
uniform mat4 uBoneMatrices[64];

out vec3 vNormal;
out vec2 vUv;
out vec3 vWorldPos;

void main() {
	vec4 localPos = vec4(aPos, 1.0);
	vec3 localNormal = aNormal;

	if (uUseSkinning) {
		vec4 w = aWeights;
		float wSum = w.x + w.y + w.z + w.w;
		mat4 skinMatrix;
		if (wSum > 1e-6) {
			w /= wSum;
			skinMatrix = w.x * uBoneMatrices[int(aJoints.x + 0.0)] + 
				w.y * uBoneMatrices[int(aJoints.y + 0.0)] +
				w.z * uBoneMatrices[int(aJoints.z + 0.0)] +
				w.w * uBoneMatrices[int(aJoints.w + 0.0)];
		} else skinMatrix = mat4(1.0);
		
		localPos = skinMatrix * localPos;
		localNormal = mat3(skinMatrix) * aNormal;
	}

	vec4 world = uModel * localPos;
	vWorldPos = world.xyz;
	vNormal = normalize(uNormalMatrix * localNormal);
	vUv = aUv;
	gl_Position = uProjection * uView * world;
}
