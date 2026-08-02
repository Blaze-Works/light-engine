#include <gl/Framebuffer.hpp>
#include <gl/glad.h>

#include <stdexcept>
#include <string>

namespace blaze::lightEngine {

Framebuffer::Framebuffer(int w, int h, bool useDepth): fbo(0), tex(0), rbo(0), w(w), h(h), useDepth(useDepth) {}

void Framebuffer::initFbo(int width, int height) {
    this->w = width;
    this->h = height;

    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    this->fbo = fbo;
    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);

    GLuint tex;
    glGenTextures(1, &tex);
    this->tex = tex;
    glBindTexture(GL_TEXTURE_2D, this->tex);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->w, this->h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->tex, 0);

    if (this->useDepth) {
        GLuint rbo;
        glGenRenderbuffers(1, &rbo);
        this->rbo = rbo;
        glBindRenderbuffer(GL_RENDERBUFFER, this->rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, this->w, this->h);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->rbo);
    } else {
        this->rbo = 0;
    }

    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glReadBuffer(GL_COLOR_ATTACHMENT0);

    this->checkFramebufferStatus();
    this->valid = true;
}

void Framebuffer::beginWrite(bool setViewport) {
    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
    if (setViewport) glViewport(0, 0, w, h);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Framebuffer::endWrite() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
}

GLuint Framebuffer::getTexture() {
    return this->tex;
}

int Framebuffer::getWidth() {
    return this->w;
}

int Framebuffer::getHeight() {
    return this->h;
}

bool Framebuffer::isValid() {
    return this->valid;
}

void Framebuffer::terminate() {
    glDeleteFramebuffers(1, &this->fbo);
    glDeleteTextures(1, &this->tex);
    if (this->rbo != 0) glDeleteRenderbuffers(1, &this->rbo);
    this->valid = false;
}

void Framebuffer::clear() {
    this->beginWrite(true);
    glClearColor(0, 0, 0, 0);
    int i = 16384;
    if (this->useDepth) {
        glClearDepth(1.0);
        i |= 256;
    }

    glClear(i);
    this->endWrite();
}

void Framebuffer::resize(int width, int height) {
    glEnable(GL_DEPTH_TEST);
    if (this->fbo != 0) {
        this->terminate();
    }
    this->valid = false;
    this->initFbo(width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::checkFramebufferStatus() {
    int i = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (i == 36053) return;

    std::string status;
    switch (i) {
        case 36055: status = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"; break;
        case 36054: status = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"; break;
        case 36059: status = "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"; break;
        case 36060: status = "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"; break;
        case 36061: status = "GL_FRAMEBUFFER_UNSUPPORTED"; break;
        case 1285:  status = "GL_OUT_OF_MEMORY"; break;
        default:    status = "glCheckFramebufferStatus returned unknown status:" + std::to_string(i); break;
    }

    throw std::runtime_error(status);
}

void Framebuffer::drawFramebufferToScreen() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(0);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, this->w, this->h, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, this->tex);

    glBegin(GL_QUADS);
    glTexCoord2f(0, 1); glVertex2f(0, 0);
    glTexCoord2f(1, 1); glVertex2f(this->w, 0);
    glTexCoord2f(1, 0); glVertex2f(this->w, this->h);
    glTexCoord2f(0, 0); glVertex2f(0, this->h);
    glEnd();

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}

} // namespacs blaze::lightEngine
