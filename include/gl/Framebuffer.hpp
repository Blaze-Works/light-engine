#pragma once

#include "glad.h"

namespace blaze::lightEngine {

class Framebuffer {
public:
    GLuint fbo;
    GLuint tex;
    GLuint rbo;

    Framebuffer(int w, int h, bool usedepth);
    void initFbo(int width, int height);
    void beginWrite(bool setViewport);
    void endWrite();
    void terminate();
    GLuint getTexture();
    int getWidth();
    int getHeight();
    bool isValid();
    void clear();
    void resize(int width, int height);
    void checkFramebufferStatus();
    void drawFramebufferToScreen();

private:
    int w;
    int h;
    bool valid;
    bool useDepth;
};

} // namespace blaze::lightEngine