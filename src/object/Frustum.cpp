#include <object/Frustum.hpp>

namespace blaze::lightEngine {

void Frustum::normalizePlane(glm::vec4& p) {
	float len = std::sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
	if (len > 1e-8f) {
		p /= len;
	}
}

void Frustum::setFromMatrix(const glm::mat4& m) {
	planes[Left]   = glm::vec4(m[0][3] + m[0][0], m[1][3] + m[1][0], m[2][3] + m[2][0], m[3][3] + m[3][0]);
	planes[Right]  = glm::vec4(m[0][3] - m[0][0], m[1][3] - m[1][0], m[2][3] - m[2][0], m[3][3] - m[3][0]);
	planes[Bottom] = glm::vec4(m[0][3] + m[0][1], m[1][3] + m[1][1], m[2][3] + m[2][1], m[3][3] + m[3][1]);
	planes[Top]    = glm::vec4(m[0][3] - m[0][1], m[1][3] - m[1][1], m[2][3] - m[2][1], m[3][3] - m[3][1]);
	planes[Near]   = glm::vec4(m[0][3] + m[0][2], m[1][3] + m[1][2], m[2][3] + m[2][2], m[3][3] + m[3][2]);
	planes[Far]    = glm::vec4(m[0][3] - m[0][2], m[1][3] - m[1][2], m[2][3] - m[2][2], m[3][3] - m[3][2]);

	for (int i = 0; i < Count; i++) {
		this->normalizePlane(planes[i]);
	}
}

bool Frustum::intersectsSphere(const glm::vec3& center, float radius) const {
	for (int i = 0; i < Count; i++) {
		const glm::vec4& p = planes[i];
		float dist = p.x * center.x + p.y * center.y + p.z * center.z + p.w;
		if (dist < -radius) return false;
	}
	return true;
}

bool Frustum::intersectsBox(const glm::vec3& min, const glm::vec3& max) const {
	for (int i = 0; i < Count; i++) {
		const glm::vec4& p = planes[i];

		glm::vec3 positive(
			p.x >= 0.0f ? max.x : min.x,
			p.y >= 0.0f ? max.y : min.y,
			p.z >= 0.0f ? max.z : min.z
		);
		float dist = p.x * positive.x + p.y * positive.y + p.z * positive.z + p.w;
		if (dist < 0.0f) return false;
	}
	return true;
}

bool Frustum::containsPoint(const glm::vec3& point) const {
	for (int i = 0; i < Count; i++) {
		const glm::vec4& p = planes[i];
		if (p.x * point.x + p.y * point.y + p.z * point.z + p.w < 0.0f) return false;
	}
	return true;
}

}; // namwspace blaze::ligjtEngine
