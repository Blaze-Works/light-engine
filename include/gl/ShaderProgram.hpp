#pragma once

#include "glad.h"

#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

namespace blaze::lightEngine {

class ShaderProgram {
public:
    ShaderProgram(std::string vertSrc, std::string fragSrc);
    void bind();
    void unbind();
    void bindTexture(GLuint texture);
    void activateTexture(GLuint texture);
    GLuint getActiveTexture();
    void enableVertexAttribute(GLint location);
    void disableVertexAttribute(GLint location);
    GLint getAttributeLocation(std::string name);
    void pointVertexAttribute(GLint location, int size, int stride, const void *pointer);
    GLint getUniformLocation(std::string name);

    void setUniform(GLint location, int value);
    void setUniform(std::string name, int value);
    void setUniform(GLint location, double value);
    void setUniform(std::string name, double value);
    void setUniform(GLint location, float value);
    void setUniform(std::string name, float value);
    void setUniform(GLint location, float i, float i1, float i2, float i3);
    void setUniform(std::string name, float i, float i1, float i2, float i3);
    void setUniform(GLint location, const glm::vec2 value);
    void setUniform(std::string name, const glm::vec2 value);
    void setUniform(GLint location, const glm::vec3 value);
    void setUniform(std::string name, const glm::vec3 value);
    void setUniform(GLint location, const glm::vec4 value);
    void setUniform(std::string name, const glm::vec4 value);
    void setUniform(GLint location, const glm::mat2 value);
    void setUniform(std::string name, const glm::mat2 value);
    void setUniform(GLint location, const glm::mat3 value);
    void setUniform(std::string name, const glm::mat3 value);
    void setUniform(GLint location, const glm::mat4 value);
    void setUniform(std::string name, const glm::mat4 value);

    const GLuint programId;

private:
    std::unordered_map<std::string, GLint> uniforms;
    GLuint activeTexture;
};

}
