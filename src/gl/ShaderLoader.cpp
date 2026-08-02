#include <gl/glad.h>
#include <gl/ShaderLoader.hpp>
#include <gl/ShaderProgram.hpp>
#include <util/PathUtil.hpp>

namespace blaze::lightEngine {

std::string ShaderLoader::shaderPath = "";

void ShaderLoader::setShaderSource(std::string path) {
    ShaderLoader::shaderPath = path;
}

GLuint ShaderLoader::compile(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        throw std::runtime_error(std::string("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n") + infoLog);
    }

    return shader;
}

ShaderProgram ShaderLoader::getShaderById(std::string id) {
    std::string vertSrc = PathUtil::readResourceFileAsString(PathUtil::join(ShaderLoader::shaderPath, id + ".vert"));
    std::string fragSrc = PathUtil::readResourceFileAsString(PathUtil::join(ShaderLoader::shaderPath, id + ".frag"));

    return ShaderProgram(vertSrc, fragSrc);
}

} // namespace blaze::lightEngine