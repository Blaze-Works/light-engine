#pragma once

#include <object/Object3D.hpp>

#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace blaze::lightEngine {

class Skin {
public:
	static constexpr int kMaxBones = 64;

	std::string name;
	std::vector<Object3D*> joints;
	std::vector<glm::mat4> inverseBindMatrices;
	std::vector<glm::mat4> boneMatrices;
	Object3D* skeleton = nullptr;

	void resize(size_t jointCount) {
		this->joints.resize(jointCount, nullptr);
		this->inverseBindMatrices.assign(jointCount, glm::mat4(1.0f));
		this->boneMatrices.assign(jointCount, glm::mat4(1.0f));
	}

	void update(const glm::mat4& meshWorld = glm::mat4(1.0f)) {
		const size_t n = this->joints.size();
		if (this->boneMatrices.size() != static_cast<size_t>(kMaxBones)) this->boneMatrices.assign(kMaxBones, glm::mat4(1.0f));

		Object3D* root = this->skeleton;
		if (!root) {
			for (Object3D* joint : this->joints) if (joint) { root = joint;break; }
		}

		if (root) {
			while (root->parent) root = root->parent;
			root->updateMatrixWorld(true);
		}

		for (Object3D* joint : joints) if (joint) joint->updateWorldMatrix(true, false);
		const glm::mat4 invMesh = glm::inverse(meshWorld);

		for (size_t i = 0; i < static_cast<size_t>(kMaxBones); ++i) {
			if (i < n && this->joints[i]) this->boneMatrices[i] = invMesh * this->joints[i]->matrixWorld * this->inverseBindMatrices[i];
			else this->boneMatrices[i] = glm::mat4(1.0f);
		}
	}

	int jointCount() const { return static_cast<int>(this->joints.size()); }
};

} // namespace blaze::lightEngine
