#pragma once
#include <gl/glad.h>

namespace blaze::lightEngine {

class Texture {
public:
	const GLuint id;
	Texture();
	~Texture() = default;
	void bind();
	void unbind();
	void setParameter(int name, int value);
	void uploadData(int width, int height, unsigned char* data);
	void uploadData(int internalFormat, int width, int height, int format, unsigned char* data);
	int getWidth();
	int getHeight();
	void setWidth(int width);
	void setHeight(int height);
	void setSize(int width, int height);

private:
	int width;
	int height;
};

} // namespace blaze::lightEngine
