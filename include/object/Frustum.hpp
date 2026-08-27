#pragma once

#include <glm/glm.hpp>

namespace blaze::lightEngine {

class Frustum {
public:
	enum Plane {
		Left = 0,
		Right,
		Bottom,
		Top,
		Near,
		Far,
		Count
	};

	glm::vec4 planes[Count];

	Frustum() = default;

	void setFromMatrix(const glm::mat4& viewProjection);
	bool intersectsSphere(const glm::vec3& center, float radius) const;
	bool intersectsBox(const glm::vec3& min, const glm::vec3& max) const;
	bool containsPoint(const glm::vec3& point) const;

private:
	static void normalizePlane(glm::vec4& p);
};

struct BoundingSphere {
	glm::vec3 center{0.0f};
	float radius = 0.0f;
	bool valid = false;
};

struct BoundingBox {
	glm::vec3 min{0.0f};
	glm::vec3 max{0.0f};
	bool valid = false;
};

}
