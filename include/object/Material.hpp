#pragma once

#include <color/Argb.hpp>
#include <texture/Texture.hpp>

#include <glm/glm.hpp>
#include <memory>
#include <string>

namespace blaze::lightEngine {

class Material {
public:
	std::string name;
	glm::vec4 color{1.0f};
	float opacity = 1.0f;
	bool transparent = false;
	bool depthTest = true;
	bool depthWrite = true;
	bool wireframe = false;
	int side = 0;

	std::shared_ptr<Texture> map;

	Material() = default;
	explicit Material(const glm::vec4& color) : color(color) {}
	explicit Material(int argb) {
		float a = Argb::getAlpha(argb) / 255.0f;
		float r = Argb::getRed(argb) / 255.0f;
		float g = Argb::getGreen(argb) / 255.0f;
		float b = Argb::getBlue(argb) / 255.0f;
		color = glm::vec4(r, g, b, a);
		opacity = a;
	}
};

} // namespace blaze::lightEngine
