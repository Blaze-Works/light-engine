#define STB_IMAGE_IMPLEMENTATION

#include <color/Argb.hpp>
#include <texture/stb_image.h>
#include <texture/TextureUtil.hpp>
#include <util/Logger.hpp>
#include <util/PathUtil.hpp>

#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace blaze::lightEngine {

int TextureUtil::MAX_SUPPORTED_TEXTURE_SIZE = -1;

int TextureUtil::maxSupportedTextureSize() {
	if (TextureUtil::MAX_SUPPORTED_TEXTURE_SIZE == -1) {
		GLint i;
		glGetIntegerv(3379, &i);

		for(int j = fmax(32768, i); j >= 1024; j >>= 1) {
			glTexImage2D(32868, 0, 6408, j, j, 0, 6408, 5121, NULL);
			GLint k;
			glGetTexLevelParameteriv(32868, 0, 4096, &k);
			if (k != 0) {
				TextureUtil::MAX_SUPPORTED_TEXTURE_SIZE = j;
				return j;
			}
		}

		TextureUtil::MAX_SUPPORTED_TEXTURE_SIZE = std::max(i, 1024);
		LOG_INFO("Failed to determine maximum texture size by probing, trying GL_MAX_TEXTURE_SIZE = " + std::to_string(MAX_SUPPORTED_TEXTURE_SIZE));
	}

	return MAX_SUPPORTED_TEXTURE_SIZE;
}

Texture TextureUtil::loadTexture(const std::string& path) {
	std::ifstream file = PathUtil::readResourceFileAsStream(path);

	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<char> buffer(size);
	if (!file.read(buffer.data(), size)) {
		throw std::runtime_error("Failed to read file resource into memory" + path);
	}

	file.close();

	stbi_set_flip_vertically_on_load(true);

	int w = 0;
	int h = 0;
	int channels = 0;

	unsigned char* image = stbi_load_from_memory(
		reinterpret_cast<const unsigned char*>(buffer.data()),
		static_cast<int>(buffer.size()),
		&w, &h, &channels, 4
	);

	if (image == nullptr) {
		throw std::runtime_error("Failed to load image: " + std::string(stbi_failure_reason()));
	}

	Texture texture = TextureUtil::createTexture(w, h, image);
	stbi_image_free(image);

	return texture;
}

GLuint TextureUtil::generateTextureId() {
	GLuint tex;
	glGenTextures(1, &tex);
	return tex;
}

Texture TextureUtil::createTexture(int width, int height, unsigned char* buffer) {
	Texture texture;
	texture.setWidth(width);
	texture.setHeight(height);

	texture.bind();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	texture.uploadData(GL_RGBA8, width, height, GL_RGBA, buffer);

	return texture;
}

Texture TextureUtil::createAlphaTexture(int width, int height, unsigned char* buffer) {
	Texture texture;
	texture.setWidth(width);
	texture.setHeight(height);

	texture.bind();

	GLint swizzleMask[] = {GL_ONE, GL_ONE, GL_ONE, GL_RED};
	glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	texture.uploadData(GL_RED, width, height, GL_RED, buffer);

	return texture;
}

Texture TextureUtil::coloredTexture(int argb) {
	return TextureUtil::coloredTexture(Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb), 1, 1);
}

Texture TextureUtil::coloredTexture(int argb, int width, int height) {
	return coloredTexture(Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb), width, height);
}

Texture TextureUtil::coloredTexture(int r, int g, int b, int a) {
	return coloredTexture(r, g, b, a, 1, 1);
}

Texture TextureUtil::coloredTexture(int r, int g, int b, int a, int width, int height) {
	unsigned char data[] = {static_cast<unsigned char>(r), static_cast<unsigned char>(g), static_cast<unsigned char>(b), static_cast<unsigned char>(a)};
	return TextureUtil::createTexture(width, height, data);
}

void TextureUtil::unpackAlignment() {
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

} // namespace blaze::lightEngine
