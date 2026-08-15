#include <gl/glad.h>
#include <gl/ShaderLoader.hpp>
#include <gl/ShaderProgram.hpp>

#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>

namespace blaze::lightEngine {

ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept: programId(other.programId), uniforms(std::move(other.uniforms)), activeTexture(other.activeTexture) {
    const_cast<GLuint&>(other.programId) = 0;
}

ShaderProgram::ShaderProgram(std::string vertSrc, std::string fragSrc): programId(glCreateProgram()) {
    GLuint vs = ShaderLoader::compile(GL_VERTEX_SHADER, vertSrc.c_str());
    GLuint fs = ShaderLoader::compile(GL_FRAGMENT_SHADER, fragSrc.c_str());

    glAttachShader(this->programId, vs);
    glAttachShader(this->programId, fs);
    glLinkProgram(this->programId);

    int success;
    char infoLog[512];
    glGetProgramiv(this->programId, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(this->programId, 512, NULL, infoLog);
        throw std::runtime_error(std::string("Shader link failed:\n") + infoLog);
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
    this->activeTexture = 0;
}

ShaderProgram::~ShaderProgram() {
    if (programId) glDeleteProgram(this->programId);
}

void ShaderProgram::bind() {
    int i = this->getActiveTexture();
    glUseProgram(this->programId);
    this->activateTexture(i);
}

void ShaderProgram::unbind() {
    int i = this->getActiveTexture();
    glUseProgram(0);
    this->activateTexture(i);
}

void ShaderProgram::bindTexture(GLuint texture) {
    glBindTexture(GL_TEXTURE_2D, texture);
}

void ShaderProgram::activateTexture(GLuint texture) {
    if (this->activeTexture != texture) {
        glActiveTexture(GL_TEXTURE0 + texture);
        this->activeTexture = texture;
    }
}

GLuint ShaderProgram::getActiveTexture() {
    return this->activeTexture;
}

void ShaderProgram::enableVertexAttribute(GLint location) {
    glEnableVertexAttribArray(location);
}

void ShaderProgram::disableVertexAttribute(GLint location) {
    glDisableVertexAttribArray(location);
}

GLint ShaderProgram::getAttributeLocation(std::string name) {
    return glGetAttribLocation(this->programId, name.c_str());
}

void ShaderProgram::pointVertexAttribute(GLint location, int size, int stride, const void *pointer) {
    glVertexAttribPointer(location, size, GL_FLOAT, false, stride, pointer);
}

GLint ShaderProgram::getUniformLocation(std::string name) {
    if (auto it = this->uniforms.find(name); it != this->uniforms.end()) {
        return it->second;
    }

    GLint loc = glGetUniformLocation(this->programId, name.c_str());
    this->uniforms[name] = loc;
    return loc;
}

void ShaderProgram::setUniform(GLint location, int value) {
    glUniform1i(location, value);
}

void ShaderProgram::setUniform(std::string name, int value) {
    this->setUniform(this->getUniformLocation(name), value);
}

void ShaderProgram::setUniform(GLint location, double value) {
    glUniform1d(location, value);
}

void ShaderProgram::setUniform(std::string name, double value) {
    this->setUniform(this->getUniformLocation(name), value);
}

void ShaderProgram::setUniform(GLint location, float value) {
    glUniform1f(location, value);
}

void ShaderProgram::setUniform(std::string name, float value) {
    this->setUniform(this->getUniformLocation(name), value);
}

void ShaderProgram::setUniform(GLint location, float i, float i1, float i2, float i3) {
    glUniform4f(location, i, i1, i2, i3);
}

void ShaderProgram::setUniform(std::string name, float i, float i1, float i2, float i3) {
    this->setUniform(this->getUniformLocation(name), i, i1, i2, i3);
}

void ShaderProgram::setUniform(GLint location, const glm::vec2 value) {
    glUniform2fv(location, 1, glm::value_ptr(value));
}

void ShaderProgram::setUniform(std::string name, const glm::vec2 value) {
    this->setUniform(this->getUniformLocation(name), value);
}

void ShaderProgram::setUniform(GLint location, const glm::vec3 value) {
    glUniform3fv(location, 1, glm::value_ptr(value));
}

void ShaderProgram::setUniform(std::string name, const glm::vec3 value) {
    this->setUniform(this->getUniformLocation(name), value);
}

void ShaderProgram::setUniform(GLint location, const glm::vec4 value) {
    glUniform4fv(location, 1, glm::value_ptr(value));
}

void ShaderProgram::setUniform(std::string name, const glm::vec4 value) {
    this->setUniform(this->getUniformLocation(name), value);
}

void ShaderProgram::setUniform(GLint location, const glm::mat2 value) {
    glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void ShaderProgram::setUniform(std::string name, const glm::mat2 value) {
    this->setUniform(this->getUniformLocation(name), value);
}

void ShaderProgram::setUniform(GLint location, const glm::mat3 value) {
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void ShaderProgram::setUniform(std::string name, const glm::mat3 value) {
    this->setUniform(this->getUniformLocation(name), value);
}

void ShaderProgram::setUniform(GLint location, const glm::mat4 value) {
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void ShaderProgram::setUniform(std::string name, const glm::mat4 value) {
    this->setUniform(this->getUniformLocation(name), value);
}

} // namespace blaze::lightEngine
