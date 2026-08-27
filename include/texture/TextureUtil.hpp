#pragma once

#include <color/Color.hpp>
#include <texture/Texture.hpp>

#include <string>

namespace blaze::lightEngine {

class TextureUtil {
public:
	static int maxSupportedTextureSize();
	static Texture loadTexture(const std::string& path);
	static Texture loadFromMemory(const unsigned char* data, int size, bool flipY = true);
	static GLuint generateTextureId();
	static Texture createTexture(int width, int height, unsigned char* buffer);
	static Texture createAlphaTexture(int width, int height, unsigned char* buffer);
	static Texture coloredTexture(int argb) { return TextureUtil::coloredTexture(Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb), 1, 1); }
	static Texture coloredTexture(Color color) { return TextureUtil::coloredTexture(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha(), 1, 1); }
	static Texture coloredTexture(int argb, int width, int height) { return coloredTexture(Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb), width, height); }
	static Texture coloredTexture(Color color, int width, int height) { return coloredTexture(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha(), width, height); }
	static Texture coloredTexture(int r, int g, int b, int a);
	static Texture coloredTexture(int r, int g, int b, int a, int width, int height);
	static void unpackAlignment();

private:
	static int MAX_SUPPORTED_TEXTURE_SIZE;
};

}
