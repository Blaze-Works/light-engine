#include <object/controls/OrbitControls.hpp>
#include <input/Keyboard.hpp>
#include <input/Mouse.hpp>

#include <glm/geometric.hpp>

namespace blaze::lightEngine {

OrbitControls::OrbitControls(Object3D& object, Window& window): object(object), window(window) {
	this->update();
	
	this->mouseDown = [this](int button, double x, double y) { this->onMouseDown(button, x, y); };
	this->mouseMove = [this](double x, double y) { this->onMouseMove(x, y); };
	this->mouseScroll = [this](double x, double y) { this->onMouseScroll(x); };
	this->mouseUp = [this](int button, double x, double y) { this->onMouseUp(x, y); };
	this->keyDown = [this](int key) { this->onKeyDown(key); };

	this->window.onMouseDown(this->mouseDown);
	this->window.onMouseScroll(this->mouseScroll);
	this->window.onKeyDown(this->keyDown);
}

float OrbitControls::getPolarAngle() const {
	return this->spherical.phi;
}

float OrbitControls::getAzimuthalAngle() const {
	return this->spherical.theta;
}

void OrbitControls::saveState() {
	this->target0 = this->target;
	this->position0 = this->object.position;
	this->zoom0 = this->object.zoom;
}

void OrbitControls::reset () {
	this->target = this->target0;
	this->object.position = this->position0;
	this->object.zoom = this->zoom0;

	// this->dispatchEvent(changeEvent);

	this->update();
	this->state = State::NONE;
}

bool OrbitControls::update() {
	glm::vec3 offset;
	glm::quat quat = {this->object.up, glm::vec3(0, 1, 0)};
	glm::quat quatInverse = glm::inverse(quat);
	glm::vec3 lastPosition;
	glm::quat lastQuaternion;

	float twoPI = (float) (2 * PI);
	glm::vec3 position = this->object.position;

	offset = position - this->target;
	offset = quat * offset;
	this->spherical.setFromVec3(offset);

	if (this->autoRotate && state == State::NONE) {
		this->rotateLeft(this->getAutoRotationAngle());
	}

	if (this->enableDamping) {
		this->spherical.theta += this->sphericalDelta.theta * this->dampingFactor;
		this->spherical.phi += this->sphericalDelta.phi * this->dampingFactor;
	} else {
		this->spherical.theta += this->sphericalDelta.theta;
		this->spherical.phi += this->sphericalDelta.phi;
	}

	float min = this->minAzimuthAngle;
	float max = this->maxAzimuthAngle;

	if (std::isfinite(min) && std::isfinite(max)) {
		if (min < - PI) min += twoPI; else if (min > PI) min -= twoPI;
		if (max < - PI) max += twoPI; else if (max > PI) max -= twoPI;
		if (min <= max) {
			this->spherical.theta = MathUtils::clamp(this->spherical.theta, min, max);
		} else {
			this->spherical.theta = (this->spherical.theta > (min + max) / 2) ? std::max(min, this->spherical.theta) : std::min(max, this->spherical.theta);
		}
	}

	this->spherical.phi = MathUtils::clamp(this->spherical.phi, this->minPolarAngle, this->maxPolarAngle * 2);
	this->spherical.makeSafe();
	this->spherical.radius *= scale;
	this->spherical.radius = MathUtils::clamp(this->spherical.radius, this->minDistance, this->maxDistance);
	
	if (this->enableDamping) {
		this->target += this->panOffset * this->dampingFactor;
	} else {
		this->target += this->panOffset;
	}

	offset = this->spherical.toVec3();
	offset = quatInverse * offset;
	position = this->target + offset;
	this->object.position = position;
	this->object.lookAt(this->target);

	if (this->enableDamping) {
		this->sphericalDelta.theta *= (1 - this->dampingFactor);
		this->sphericalDelta.phi *= (1 - this->dampingFactor);
		this->panOffset *= (1 - this->dampingFactor);
	} else {
		this->sphericalDelta.set(0, 0, 0);
		this->panOffset = {0, 0, 0};
	}

	this->scale = 1;

	if (this->zoomChanged || glm::distance2(lastPosition, this->object.position) > EPS || 8 * (1 - glm::dot(lastQuaternion, this->object.quaternion)) > EPS) {
		// this->dispatchEvent(this->changeEvent);
		lastPosition = this->object.position;
		lastQuaternion = this->object.quaternion;
		this->zoomChanged = false;

		return true;
	}
	return false;
}

void OrbitControls::dispose() {
	this->window.offMouseDown(this->mouseDown);
	this->window.offMouseScroll(this->mouseScroll);
	this->window.offMouseMove(this->mouseMove);
	this->window.offMouseUp(this->mouseUp);
	this->window.offKeyDown(this->keyDown);
}

void OrbitControls::rotateLeft(float angle) {
	this->sphericalDelta.theta -= angle;
}

void OrbitControls::rotateUp(float angle) {
	this->sphericalDelta.phi -= angle;
}

void OrbitControls::panLeft(float distance, glm::mat4 objectMatrix) {
	glm::vec3 v = glm::vec3(objectMatrix[0]);
	v *= distance;
	this->panOffset += v;
}

void OrbitControls::panUp(float distance, glm::mat4 objectMatrix) {
	glm::vec3 v;
	if (this->screenSpacePanning) {
		v = glm::vec3(objectMatrix[1]);
	} else {
		v = glm::vec3(objectMatrix[0]);
		v = this->object.up * v;
	}
	v *= distance;
	this->panOffset += v;
}

void OrbitControls::pan(float deltaX, float deltaY) {
	glm::vec3 offset;
	if (PerspectiveCamera* camera = dynamic_cast<PerspectiveCamera*>(&this->object)) {
		glm::vec3 position = camera->position;
		offset = position - this->target;

		float targetDistance = offset.length();
		targetDistance *= (float) std::tan((camera->fov / 2) * PI / 180.0);

		this->panLeft(-2 * deltaX * targetDistance / this->window.getWidth(), this->object.matrix);
		this->panUp(2 * deltaY * targetDistance / this->window.getHeight(), this->object.matrix);
	} /*else if (OrthographicCamera* camera = dynamic_cast<OrthographicCamera*>) {
		panLeft(deltaX * (camera->right - camera->left) / this->object.zoom / this->window.getWidth(), this->object.matrix);
		panUp(deltaY * (camera->top - camera->bottom) / this->object.zoom / this->window.getHeight, this->object.matrix);
	}*/
}

void OrbitControls::dollyOut(float dollyScale) {
	if (PerspectiveCamera* _ = dynamic_cast<PerspectiveCamera*>(&this->object)) {
		this->scale /= dollyScale;
	} /*else if (OrthographicCamera* _ = dynamic_cast<OrthographicCamera*>) {
		this->object.zoom = MathUtils::clamp(this->object.zoom * dollyScale, this->minZoom, this->maxZoom);
		this->object.updateProjectionMatrix();
		this->zoomChanged = true;
	}*/

}

void OrbitControls::dollyIn(float dollyScale) {
	if (PerspectiveCamera* _ = dynamic_cast<PerspectiveCamera*>(&this->object)) {
		this->scale *= dollyScale;
	} /*else if (OrthographicCamera* _ = dynamic_cast<OrthographicCamera*>) {
		this->object.zoom = MathUtils::clamp(this->object.zoom / dollyScale, this->minZoom, this->maxZoom);
		this->object.updateProjectionMatrix();
		this->zoomChanged = true;
	}*/
}

void OrbitControls::handleMouseDownRotate(float mouseX, float mouseY) {
	this->rotateStart = {mouseX, mouseY};
}

void OrbitControls::handleMouseDownDolly(float mouseX, float mouseY) {
	this->dollyStart = {mouseX, mouseY};
}

void OrbitControls::handleMouseDownPan(float mouseX, float mouseY) {
	this->panStart = {mouseX, mouseY};
}

void OrbitControls::handleMouseMoveRotate(float mouseX, float mouseY) {
	this->rotateEnd = {mouseX, mouseY};
	this->rotateDelta = (this->rotateEnd - this->rotateStart) * this->rotateSpeed;

	this->rotateLeft((float) (2 * PI * this->rotateDelta.x / this->window.getWidth()));
	this->rotateUp((float) (2 * PI * this->rotateDelta.y / this->window.getHeight()));

	this->rotateStart = this->rotateEnd;
	this->update();
}

void OrbitControls::handleMouseMoveDolly(float mouseX, float mouseY) {
	this->dollyEnd = {mouseX, mouseY};
	this->dollyDelta = this->dollyEnd - this->dollyStart;

	if (this->dollyDelta.y > 0) {
		this->dollyOut(this->getZoomScale());
	} else if (dollyDelta.y < 0) {
		this->dollyIn(this->getZoomScale());
	}

	this->dollyStart = this->dollyEnd;
	this->update();
}

void OrbitControls::handleMouseMovePan(float mouseX, float mouseY) {
	this->panEnd = {mouseX, mouseY};

	this->panDelta = (this->panEnd - this->panStart) * this->panSpeed;
	this->pan(this->panDelta.x, this->panDelta.y);

	this->panStart = this->panEnd;
	this->update();
}

void OrbitControls::handleMouseUp(float mouseX, float mouseY) {
	// no-op
}

void OrbitControls::handleMouseScroll(float deltaY) {
	if (deltaY < 0) {
		this->dollyIn(this->getZoomScale());
	} else if (deltaY > 0) {
		this->dollyOut(this->getZoomScale());
	}
	this->update();
}

void OrbitControls::onMouseDown(int mouseButton, double x, double y) {
	float mouseX = (float) x;
	float mouseY = (float) y;

	switch (mouseButton) {
		case Mouse::Middle:
			if (!this->enableZoom) return;
			this->handleMouseDownDolly(mouseX, mouseY);
			this->state = State::DOLLY;
			break;

		case Mouse::Left:
			if (Keyboard::isPressed(Keyboard::LeftCtrl) || Keyboard::isPressed(Keyboard::LeftShift)) {
				if (!this->enablePan) return;
				this->handleMouseDownPan(mouseX, mouseY);
				this->state = State::PAN;
			} else {
				if (!this->enableRotate) return;
				this->handleMouseDownRotate(mouseX, mouseY);
				this->state = State::ROTATE;
			}
			break;

		case Mouse::Right:
			if (Keyboard::isPressed(Keyboard::LeftCtrl) || Keyboard::isPressed(Keyboard::LeftShift)) {
				if (!this->enableRotate) return;
				this->handleMouseDownRotate(mouseX, mouseY);
				this->state = State::ROTATE;
			} else {
				if (!this->enablePan) return;
				this->handleMouseDownPan(mouseX, mouseY);
				this->state = State::PAN;
			}
			break;
	}

	if (this->state != State::NONE) {
		this->window.onMouseMove(this->mouseMove);
		this->window.onMouseUp(this->mouseUp);
		// this->dispatchEvent(this->startEvent);
	}
}

void OrbitControls::onMouseMove(double x, double y) {
	if (!this->enabled) return;

	float mouseX = (float) x;
	float mouseY = (float) y;
	
	switch (this->state) {
		case State::ROTATE:
			if (!this->enableRotate) return;
			this->handleMouseMoveRotate(mouseX, mouseY);
			break;

		case State::DOLLY:
			if (!this->enableZoom) return;
			this->handleMouseMoveDolly(mouseX, mouseY);
			break;

		case State::PAN:
			if (!this->enablePan) return;
			this->handleMouseMovePan(mouseX, mouseY);
			break;

		default: break;
	}

}

void OrbitControls::onMouseUp(double mouseX, double mouseY) {
	if (!this->enabled) return;

	this->window.offMouseMove(this->mouseMove);
	this->window.offMouseUp(this->mouseUp);

	this->handleMouseUp((float) mouseX, (float) mouseY);
	// this->dispatchEvent(this->endEvent);
	this->state = State::NONE;
}

void OrbitControls::onMouseScroll(double mouseScroll) {
	if (!this->enabled || !this->enableZoom || (state != State::NONE && state != State::ROTATE)) return;
	// this->dispatchEvent(startEvent);
	this->handleMouseScroll((float) mouseScroll);
	// this->dispatchEvent(endEvent);
}

void OrbitControls::onKeyDown(int key) {
	// no-op
}

} // namespace blaze::lightEngine
