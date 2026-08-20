#pragma once

#include <string>

namespace blaze::lightEngine {

struct Fog {
	std::string name;
	int color = 0x000000;
	float near = 1.0f;
	float far = 1000.0f;

	Fog() = default;
	Fog(int color, float near = 1.0f, float far = 1000.0f): color(color), near(near), far(far) {}
};

struct FogExp2 {
	std::string name;
	int color = 0x000000;
	float density = 0.00025f;

	FogExp2() = default;
	FogExp2(int color, float density = 0.00025f): color(color), density(density) {}
};

} // namespace blaze::lightEngine
