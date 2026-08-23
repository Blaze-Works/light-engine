#pragma once

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
	static Texture coloredTexture(int argb);
	static Texture coloredTexture(int argb, int width, int height);
	static Texture coloredTexture(int r, int g, int b, int a);
	static Texture coloredTexture(int r, int g, int b, int a, int width, int height);
	static void unpackAlignment();

private:
	static int MAX_SUPPORTED_TEXTURE_SIZE;
};

}
