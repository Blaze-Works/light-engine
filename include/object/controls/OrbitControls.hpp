#pragma once

#include <object/camera/PerspectiveCamera.hpp>
#include <util/Window.hpp>
#include <util/math/MathUtils.hpp>

#include <cmath>
#include <limits>

namespace blaze::lightEngine {

class Spherical {
public:
	float phi;
	float theta;
	float radius;

	Spherical(float radius, float phi, float theta): radius(radius), phi(phi), theta(theta) {}

	void set(float radius, float phi, float theta) {
		this->radius = radius;
		this->phi = phi;
		this->theta = theta;
	}

	glm::vec3 toVec3() {
		glm::vec3 out;
		float sinPhiRadius = (float) (std::sin(phi) * radius);
        out.x = sinPhiRadius  * std::sin(theta);
        out.y = std::cos(phi) * radius;
        out.z = sinPhiRadius  * std::cos(theta);
		return out;
	}

	void setFromVec3(glm::vec3 v) {
        this->setFromCartesianCoords(v.x, v.y, v.z);
    }

    void setFromCartesianCoords(float x, float y, float z) {
        this->radius = std::sqrt(x * x + y * y + z * z);

        if (this->radius == 0) {
            this->theta = 0;
            this->phi = 0;
        } else {
            this->theta = std::atan2(x, z);
            this->phi = std::acos(MathUtils::clamp(y / this->radius, -1.0f, 1.0f));
        }
    }

	void makeSafe() {
		float EPS = 0.000001F;
        this->phi = MathUtils::clamp(this->phi, EPS, (float)PI - EPS);
	}
};

class OrbitControls {
public:
	OrbitControls(Object3D& camera, Window& window);
	~OrbitControls() = default;

	float getPolarAngle() const;
	float getAzimuthalAngle() const;
	void saveState();
	void reset();
	bool update();
	void dispose();
	float getAutoRotationAngle() { return (float) (2 * PI / 60 / 60 * this->autoRotateSpeed); }
	float getZoomScale() { return std::pow(0.95, this->zoomSpeed); }

	void rotateLeft(float angle);
	void rotateUp(float angle);
	void panLeft(float distance, glm::mat4 m);
	void panUp(float distance, glm::mat4 m);
	void pan(float deltaX, float deltaY);
	void dollyOut(float dollyScale);
	void dollyIn(float dollyScale);

	WindowMouseDownfun mouseDown;
	WindowMouseScrollfun mouseScroll;
	WindowMouseMovefun mouseMove;
	WindowMouseUpfun mouseUp;
	WindowKeyDownfun keyDown;

	void onMouseDown(int button, double x, double y);
	void onMouseMove(double x, double y);
	void onMouseScroll(double mouseScroll);
	void onMouseUp(double x, double y);
	void onKeyDown(int key);

	void handleMouseDownRotate(float mouseX, float mouseY);
	void handleMouseDownDolly(float mouseX, float mouseY);
	void handleMouseDownPan(float mouseX, float mouseY);
	void handleMouseMoveRotate(float mouseX, float mouseY);
	void handleMouseMoveDolly(float mouseX, float mouseY);
	void handleMouseMovePan(float mouseX, float mouseY);
	void handleMouseUp(float mouseX, float mouseY);
	void handleMouseScroll(float delta);

	Object3D& object;
	Window& window;
	glm::vec3 target;
	bool enabled = true;
	float minDistance = 0;
	float maxDistance = std::numeric_limits<float>().max();
	float minZoom = 0;
	float maxZoom = std::numeric_limits<float>().max();
	float minPolarAngle = 0;
	float maxPolarAngle = (float) PI;
	float minAzimuthAngle = -(std::numeric_limits<float>().max());
	float maxAzimuthAngle = std::numeric_limits<float>().max();
	bool enableDamping = false;
	float dampingFactor = 0.05f;
	bool enableZoom = true;
	float zoomSpeed = 1.0f;
	bool enableRotate = true;
	float rotateSpeed = 1.0f;
	bool enablePan = true;
	float panSpeed = 1.0f;
	bool screenSpacePanning = true;
	float keyPanSpeed = 7.0f;
	bool autoRotate = false;
	float autoRotateSpeed = 2.0f;
	bool enableKeys = true;
	glm::vec3 target0;
	glm::vec3 position0;
	glm::vec3 zoom0;
	
private:
	enum State {
		NONE               = -1,
		ROTATE             = 0,
		DOLLY              = 1,
		PAN                = 2,
		TOUCH_ROTATE       = 3,
		TOUCH_PAN          = 4,
		TOUCH_DOLLY_PAN    = 5,
		TOUCH_DOLLY_ROTATE = 6
	};

	State state = State::NONE;

    float EPS = 0.000001f;
	float scale = 1;
    bool zoomChanged = false;
    Spherical spherical = Spherical(0, 0, 0);
    Spherical sphericalDelta = Spherical(0, 0, 0);
	
    glm::vec2 rotateStart;
    glm::vec2 rotateEnd;
	glm::vec2 rotateDelta;
    glm::vec3 panOffset;
    glm::vec2 panStart;
    glm::vec2 panEnd;
    glm::vec2 panDelta;
    glm::vec2 dollyStart;
    glm::vec2 dollyEnd;
    glm::vec2 dollyDelta;
	
};

} // namespace blaze::lightEngine