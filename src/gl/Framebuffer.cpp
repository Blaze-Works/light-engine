#include <gl/Framebuffer.hpp>
#include <gl/glad.h>
#include <gl/ShaderPrograms.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <stdexcept>
#include <string>

namespace blaze::lightEngine {

Framebuffer::Framebuffer(int w, int h, bool useDepth): fbo(0), tex(0), rbo(0), w(w), h(h), useDepth(useDepth) {}

Framebuffer::~Framebuffer() {
    this->terminate();
}

void Framebuffer::initFbo(int width, int height) {
    this->w = width;
    this->h = height;

    this->setShader(ShaderPrograms::POSITION_TEX);

    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    this->fbo = fbo;
    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);

    GLuint tex;
    glGenTextures(1, &tex);
    this->tex = tex;
    glBindTexture(GL_TEXTURE_2D, this->tex);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, this->w, this->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
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

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
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

void Framebuffer::terminate() {
    glDeleteFramebuffers(1, &this->fbo);
    glDeleteTextures(1, &this->tex);
    if (this->rbo != 0) glDeleteRenderbuffers(1, &this->rbo);
    this->valid = false;
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

void Framebuffer::setShader(std::shared_ptr<ShaderProgram> shader) {
    this->blitShader = shader;

    glGenVertexArrays(1, &this->blitVao);
    glGenBuffers(1, &this->blitVbo);
    glBindVertexArray(this->blitVao);
    glBindBuffer(GL_ARRAY_BUFFER, this->blitVbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
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

    // static std::unique_ptr<ShaderProgram> blitShader;
    // static GLuint blitVao = 0;
    // static GLuint blitVbo = 0;

    if (!blitShader) {
        const std::string vertexSrc =
            "#version 330 core\n"
            "layout(location = 0) in vec2 aPos;\n"
            "layout(location = 1) in vec2 aUv;\n"
            "uniform mat4 uProjection;\n"
            "out vec2 vUv;\n"
            "void main() {\n"
            "    vUv = aUv;\n"
            "    gl_Position = uProjection * vec4(aPos, 0.0, 1.0);\n"
            "}\n";

        const std::string fragmentSrc =
            "#version 330 core\n"
            "in vec2 vUv;\n"
            "uniform sampler2D uTexture;\n"
            "out vec4 fragColor;\n"
            "void main() {\n"
            "    fragColor = texture(uTexture, vUv);\n"
            "}\n";

        // blitShader = std::move(ShaderPrograms::POSITION_TEX); //std::make_unique<ShaderProgram>(vertexSrc, fragmentSrc);
    }

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    const float vertices[] = {
        0.0f, 0.0f, 0.0f, 1.0f,
        0.0f, static_cast<float>(this->h), 0.0f, 0.0f,
        static_cast<float>(this->w), static_cast<float>(this->h), 1.0f, 0.0f,

        static_cast<float>(this->w), static_cast<float>(this->h), 1.0f, 0.0f,
        static_cast<float>(this->w), 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    };

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->w), static_cast<float>(this->h), 0.0f, -1.0f, 1.0f);

    this->blitShader->bind();
    this->blitShader->setUniform("uProjection", projection);
    this->blitShader->setUniform("uTexture", 0);
    this->blitShader->setUniform("uColor", 1.0f, 1.0f, 1.0f, 1.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->tex);

    glBindVertexArray(this->blitVao);
    glBindBuffer(GL_ARRAY_BUFFER, this->blitVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    this->blitShader->unbind();
}

} // namespacs blaze::lightEngine
