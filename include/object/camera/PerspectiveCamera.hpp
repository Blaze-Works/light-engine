#pragma once

#include <object/camera/Camera.hpp>

#include <optional>

namespace blaze::lightEngine {

class PerspectiveCamera : public Camera {
public:
	struct ViewOffset {
		bool enabled = false;
		float fullWidth = 1.0f;
		float fullHeight = 1.0f;
		float offsetX = 0.0f;
		float offsetY = 0.0f;
		float width = 1.0f;
		float height = 1.0f;
	};

	float fov = 50.0f;
	float zoom = 1.0f;
	float near = 0.1f;
	float far = 2000.0f;
	float focus = 10.0f;
	float aspect = 1.0f;
	float filmGauge = 35.0f;
	float filmOffset = 0.0f;

	std::optional<ViewOffset> view;

	PerspectiveCamera();
	PerspectiveCamera(float fov, float aspect, float near, float far);

	void setFocalLength(float focalLength);
	float getFocalLength() const;
	float getEffectiveFOV() const;
	float getFilmWidth() const;
	float getFilmHeight() const;

	void setAspect(float aspect);
	void setViewOffset(float fullWidth, float fullHeight, float x, float y, float width, float height);
	void clearViewOffset();
	void updateProjectionMatrix();
};

} // namespace blaze::lightEngine
