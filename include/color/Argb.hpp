#pragma once

namespace blaze::lightEngine {

class Argb {
public:
	static int getAlpha(int argb) { return (argb >> 24) & 0xFF; }
	static int getRed(int argb) { return (argb >> 16) & 0xFF; }
	static int getGreen(int argb) { return (argb >> 8) & 0xFF; }
	static int getBlue(int argb) { return argb & 0xFF; }
	static int getArgb(int alpha, int red, int green, int blue) { return (alpha << 24) | (red << 16) | (green << 8) | blue; }
	
	static int mixColor(int first, int second) {
		int alpha = (getAlpha(first) + getAlpha(second)) / 2;
		int red = (getRed(first) + getRed(second)) / 2;
		int green = (getGreen(first) + getGreen(second)) / 2;
		int blue = (getBlue(first) + getBlue(second)) / 2;
	
		return getArgb(alpha, red, green, blue);
	}
	
	static int lerp(float delta, int start, int end);
};

} // namespace blaze::lightEngine;