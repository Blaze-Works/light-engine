#include <texture/Texture.hpp>
#include <texture/TextureUtil.hpp>

namespace blaze::lightEngine {

Texture::Texture() : id(TextureUtil::generateTextureId()), width(0), height(0) {}

void Texture::bind() {
    glBindTexture(GL_TEXTURE_2D, this->id);
}

void Texture::unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::setParameter(int name, int value) {
    glTexParameteri(GL_TEXTURE_2D, name, value);
}

void Texture::uploadData(int width, int height, unsigned char* data) {
    this->uploadData(GL_RED, width, height, GL_RED, data);
}

void Texture::uploadData(int internalFormat, int width, int height, int format, unsigned char* data) {
    this->setSize(width, height);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
}

int Texture::getWidth() {
    return this->width;
}

int Texture::getHeight() {
    return this->height;
}

void Texture::setWidth(int width) {
    this->width = width;
}

void Texture::setHeight(int height) {
    this->height = height;
}

void Texture::setSize(int width, int height) {
    this->setWidth(width);
    this->setHeight(height);
}

} // namespace blaze::lightEngine
