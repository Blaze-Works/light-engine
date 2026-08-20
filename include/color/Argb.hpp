#pragma once

namespace blaze::lightEngine {

class Argb {
public:
	static int getAlpha(int argb);
	static int getRed(int argb);
	static int getGreen(int argb);
	static int getBlue(int argb);
	static int getArgb(int alpha, int red, int green, int blue);
	static int mixColor(int first, int second);
	static int lerp(float delta, int start, int end);
};

} // namespace blaze::lightEngine;