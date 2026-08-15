#pragma once

#include <gl/glad.h>

#include <string>

namespace blaze::lightEngine {

class ShaderProgram;

class ShaderLoader {
public:
    static void setShaderSource(std::string path);
    static GLuint compile(GLenum type, const char* source);
    static ShaderProgram getShaderById(std::string id);

private:
    static std::string shaderPath;
};

} // blaze::lightEngine
