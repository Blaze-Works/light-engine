#pragma once

#include <object/Object3D.hpp>

#include <glm/glm.hpp>

namespace blaze::lightEngine {

class Camera : public Object3D {
public:
	Camera();
	~Camera() override = default;

	glm::mat4 matrixWorldInverse{1.0f};
	glm::mat4 projectionMatrix{1.0f};
	glm::mat4 projectionMatrixInverse{1.0f};

	void updateMatrixWorld(bool force = false);
	void updateWorldMatrix(bool updateParents, bool updateChildren);

	glm::mat4 getViewProjectionMatrix() const {
		return projectionMatrix * matrixWorldInverse;
	}

	const glm::mat4& getProjectionMatrix() const { return projectionMatrix; }
	const glm::mat4& getProjectionMatrixInverse() const { return projectionMatrixInverse; }
	const glm::mat4& getMatrixWorldInverse() const { return matrixWorldInverse; }
};

} // namespace blaze::lightEngine