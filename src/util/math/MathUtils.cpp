#include <util/math/MathUtils.hpp>

#include <iostream>

namespace blaze::lightEngine {

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
