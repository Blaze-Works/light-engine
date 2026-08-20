#pragma once

#include <math.h>
#include <string>

namespace blaze::lightEngine {

#define PI 3.14159265358979323846
#define DEG2RAD PI / 180
#define RAD2DEG 180 / PI
#define EPSILON pow(2, -52);

class MathUtils {
public:
	static std::string generateUUID();
	static int sign(int x);
	static double sign(double x);
	static float sign(float x);
	static float clamp(float value, float min, float max);
	static double clamp(double value, double min, double max);
	static int clamp(int value, int min, int max);
};

} // namespace blaze::lightEngind