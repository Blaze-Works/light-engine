#pragma once

#include <gl/glad.h>
#include <gl/ShaderProgram.hpp>

namespace blaze::lightEngine {

class Framebuffer {
public:
	GLuint fbo;
	GLuint tex;
	GLuint rbo;
	GLuint blitVao;
	GLuint blitVbo;

	Framebuffer(int w, int h, bool usedepth);
	~Framebuffer() = default;

	void initFbo(int width, int height);
	void beginWrite(bool setViewport);
	void endWrite();
	void terminate();
	GLuint getTexture();
	int getWidth();
	int getHeight();
	bool isValid();
	void setShader(std::shared_ptr<ShaderProgram> shader);
	void clear();
	void resize(int width, int height);
	void checkFramebufferStatus();
	void drawFramebufferToScreen();

private:
	int w;
	int h;
	bool valid;
	bool useDepth;
	std::shared_ptr<ShaderProgram> blitShader;
};

} // namespace blaze::lightEngine
