#include "color/Argb.hpp"

namespace blaze::lightEngine {

int Argb::getAlpha(int argb) {
    return (argb >> 24) & 0xFF;
}

int Argb::getRed(int argb) {
    return (argb >> 16) & 0xFF;
}

int Argb::getGreen(int argb) {
    return (argb >> 8) & 0xFF;
}

int Argb::getBlue(int argb) {
    return argb & 0xFF;
}

int Argb::getArgb(int alpha, int red, int green, int blue) {
    return (alpha << 24) | (red << 16) | (green << 8) | blue;
}

int Argb::mixColor(int first, int second) {
    int alpha = (getAlpha(first) + getAlpha(second)) / 2;
    int red = (getRed(first) + getRed(second)) / 2;
    int green = (getGreen(first) + getGreen(second)) / 2;
    int blue = (getBlue(first) + getBlue(second)) / 2;

    return getArgb(alpha, red, green, blue);
}

} // namespace blaze::lightEngine