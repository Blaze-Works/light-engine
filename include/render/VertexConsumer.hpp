#pragma once

#include <color/Argb.hpp>

namespace blaze::lightEngine {

class VertexConsumer {
public:
	VertexConsumer() = default;
	virtual VertexConsumer vertex(float x, float y, float z);
	virtual VertexConsumer color(float red, float green, float blue, float alpha);
	virtual VertexConsumer texture(float u, float v);
	virtual VertexConsumer overlay(float u, float v);
	virtual VertexConsumer light(int u, int v);
	virtual VertexConsumer normal(float x, float y, float z);
	virtual void next();
	
	virtual void vertex(float x, float y, float z, float red, float green, float blue, float alpha, float u, float v, int overlay, int light, float normalX, float normalY, float normalZ) {
		this->vertex(x, y, z);
		this->color(red, green, blue, alpha);
		this->texture(u, v);
		this->overlay(overlay);
		this->light(light);
		this->normal(normalX, normalY, normalZ);
		this->next();
	}

	virtual VertexConsumer color(int red, int green, int blue, int alpha) {
		return this->color(red / 255.0f, green / 255.0f, blue / 255.0f, alpha / 255.0f);
	};

	virtual VertexConsumer color(int argb) {
		return this->color(Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb));
	}

	virtual VertexConsumer light(int uv) {
		return this->light(uv & 0xFFFF, uv >> 16 & 0xFFFF);
	}

	virtual VertexConsumer overlay(int uv) {
		return this->overlay(uv & 0xFFFF, uv >> 16 & 0xFFFF);
	}
};

} // namespace blaze::lightEngine