#include <object/camera/PerspectiveCamera.hpp>
#include <util/math/MathUtils.hpp>

#include <algorithm>
#include <cmath>

namespace blaze::lightEngine {

PerspectiveCamera::PerspectiveCamera() {
	updateProjectionMatrix();
}

PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float near, float far): fov(fov), near(near), far(far), aspect(aspect) {
	updateProjectionMatrix();
}

void PerspectiveCamera::setFocalLength(float focalLength) {
	// http://www.bobatkins.com/photography/technical/field_of_view.html
	float vExtentSlope = 0.5f * getFilmHeight() / focalLength;
	fov = RAD2DEG * 2.0f * std::atan(vExtentSlope);
	updateProjectionMatrix();
}

float PerspectiveCamera::getFocalLength() const {
	float vExtentSlope = std::tan(DEG2RAD * 0.5f * fov);
	return 0.5f * getFilmHeight() / vExtentSlope;
}

float PerspectiveCamera::getEffectiveFOV() const {
	return RAD2DEG * 2.0f * std::atan(std::tan(DEG2RAD * 0.5f * fov) / zoom);
}

float PerspectiveCamera::getFilmWidth() const {
	return filmGauge * std::min(aspect, 1.0f);
}

float PerspectiveCamera::getFilmHeight() const {
	return filmGauge / std::max(aspect, 1.0f);
}

void PerspectiveCamera::setAspect(float a) {
	aspect = a;
	updateProjectionMatrix();
}

void PerspectiveCamera::setViewOffset(float fullWidth, float fullHeight, float x, float y, float width, float height) {
	aspect = fullWidth / fullHeight;

	ViewOffset v;
	v.enabled = true;
	v.fullWidth = fullWidth;
	v.fullHeight = fullHeight;
	v.offsetX = x;
	v.offsetY = y;
	v.width = width;
	v.height = height;
	view = v;

	updateProjectionMatrix();
}

void PerspectiveCamera::clearViewOffset() {
	if (view) {
		view->enabled = false;
	}
	updateProjectionMatrix();
}

void PerspectiveCamera::updateProjectionMatrix() {
	float top = near * std::tan(DEG2RAD * 0.5f * fov) / zoom;
	float height = 2.0f * top;
	float width = aspect * height;
	float left = -0.5f * width;

	if (view && view->enabled) {
		const ViewOffset& v = *view;
		left += v.offsetX * width / v.fullWidth;
		top -= v.offsetY * height / v.fullHeight;
		width *= v.width / v.fullWidth;
		height *= v.height / v.fullHeight;
	}

	float skew = filmOffset;
	if (skew != 0.0f) {
		left += near * skew / getFilmWidth();
	}

	float right = left + width;
	float bottom = top - height;

	projectionMatrix = glm::frustum(left, right, bottom, top, near, far);
	projectionMatrixInverse = glm::inverse(projectionMatrix);
}

} // namespace blaze::lightEngine
