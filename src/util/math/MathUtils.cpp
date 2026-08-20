#include <util/math/MathUtils.hpp>

#include <algorithm>
#include <random>

namespace blaze::lightEngine {


std::string MathUtils::generateUUID() {
	static thread_local std::mt19937 rng{std::random_device{}()};
	static thread_local std::uniform_int_distribution<int> dist(0, 15);
	const char* hex = "0123456789abcdef";
	std::string out;
	out.reserve(36);
	for (int i = 0; i < 36; i++) {
		if (i == 8 || i == 13 || i == 18 || i == 23) out.push_back('-');
		else out.push_back(hex[dist(rng)]);
	}
	return out;
}

int MathUtils::sign(int x) {
	return x < 0 ? -1 : x > 0 ? 1 : +x;
}

double MathUtils::sign(double x) {
	return x < 0 ? -1 : x > 0 ? 1 : +x;
}

float MathUtils::sign(float x) {
	return x < 0 ? -1 : x > 0 ? 1 : +x;
}

int MathUtils::clamp(int value, int min, int max) {
	return std::max(min, std::min(max, value));
}

double MathUtils::clamp(double value, double min, double max) {
	return std::max(min, std::min(max, value));
}

float MathUtils::clamp(float value, float min, float max) {
	return std::max(min, std::min(max, value));
}

}
