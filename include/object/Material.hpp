#pragma once

#include <color/Color.hpp>
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
	explicit Material(Color& color) : color(color.getColor() / 255.0f) {}
	explicit Material(int argb) {
		Color color(argb);
		this->color = color.getColor() / 255.0f;
		this->opacity = color.getAlpha() / 255.0f;;
	}

};

} // namespace blaze::lightEngine
