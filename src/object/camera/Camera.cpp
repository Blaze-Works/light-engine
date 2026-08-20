#include <object/camera/Camera.hpp>

namespace blaze::lightEngine {

Camera::Camera() {
	isCameraType = true;
}

void Camera::updateMatrixWorld(bool force) {
	Object3D::updateMatrixWorld(force);
	matrixWorldInverse = glm::inverse(this->matrixWorld);
}

void Camera::updateWorldMatrix(bool updateParents, bool updateChildren) {
	Object3D::updateWorldMatrix(updateParents, updateChildren);
	matrixWorldInverse = glm::inverse(matrixWorld);
}

} // namespace blaze::lightEngine
