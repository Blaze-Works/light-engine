#pragma once

#include "Framebuffer.hpp"

#include <string>
#include <vector>

namespace blaze::lightEngine {

enum Attachment {
    NONE        = 0,
    COLOR       = 1,
    DEPTH       = 2,
    COLOR_DEPTH = 3
};

struct Size {
    int width;
    int height;

    Size(int width, int height);
    static std::vector<Size> findCompatible(int width, int height);
    bool equals(Size other);
    std::string toString();
};

class WindowFramebuffer: public Framebuffer {
public:
    WindowFramebuffer(int width, int height);

    static const int DEFAULT_WIDTH = 854;
    static const int DEFAULT_HEIGHT = 480;

private:
    void init(int width, int height);
    Size* findSuitableSize(int width, int height);
    bool supportsColor(Size* size);
    bool supportsDepth(Size* size);

    GLuint colorAttachment;
    GLuint depthAttachment;
    int viewportWidth;
    int viewportHeight;
    int textureWidth;
    int textureHeight;
};

} // namespace blaze::lightEngine
