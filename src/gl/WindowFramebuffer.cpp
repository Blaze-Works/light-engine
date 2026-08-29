#include <gl/WindowFramebuffer.hpp>
#include <texture/TextureUtil.hpp>

#include <stdexcept>
#include <string>

namespace blaze::lightEngine {

Size::Size(int width, int height): width(width), height(height) {}

Size DEFAULT(WindowFramebuffer::DEFAULT_WIDTH, WindowFramebuffer::DEFAULT_HEIGHT);

std::vector<Size> Size::findCompatible(int width, int height) {
	int i = TextureUtil::maxSupportedTextureSize();
	if (width > 0 && width <= i && height > 0 && height <= i) {
		Size size(width, height);
		return std::vector<Size>{size, DEFAULT};
	}
	return std::vector<Size>{DEFAULT};
}

std::string Size::toString() {
	return std::to_string(this->width) + "x" + std::to_string(this->height);
}

WindowFramebuffer::WindowFramebuffer(int width, int height): Framebuffer(width, height, true) {}

void WindowFramebuffer::init(int width, int height) {
	Size* size;
	size = this->findSuitableSize(width, height);

	this->viewportWidth = size->width;
	this->viewportHeight = size->height;
	this->textureWidth = size->width;
	this->textureHeight = size->height;
	this->initFbo(size->width, size->height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Size* WindowFramebuffer::findSuitableSize(int width, int height) {
	this->colorAttachment = TextureUtil::generateTextureId();
	this->depthAttachment = TextureUtil::generateTextureId();
	std::vector<Size> vec = Size::findCompatible(width, height);
	Attachment attachment = Attachment::NONE;
	auto it = vec.begin();

	Size* size = nullptr;

	while (it != vec.end()) {
		size = &*it;
		++it;
		attachment = Attachment::NONE;

		if (this->supportsColor(size)) attachment = AttachmentHelper::with(attachment, Attachment::COLOR);
		if (this->supportsDepth(size)) attachment = AttachmentHelper::with(attachment, Attachment::DEPTH);

		if (attachment == Attachment::COLOR_DEPTH) {
			return size;
		}
	}

	throw std::runtime_error("Unrecoverable GL_OUT_OF_MEMORY (allocated attachments = " + AttachmentHelper::name(attachment) + ")");
	return size;
}

bool WindowFramebuffer::supportsColor(Size* size) {
	glGetError();
	glBindTexture(GL_TEXTURE_2D, this->colorAttachment);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size->width, size->height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	return glGetError() == GL_NO_ERROR;
}

bool WindowFramebuffer::supportsDepth(Size* size) {
	glGetError();
	glBindTexture(GL_TEXTURE_2D, this->depthAttachment);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, size->width, size->height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);
	return glGetError() == GL_NO_ERROR;
}

} // namespace blaze::lightEngine
