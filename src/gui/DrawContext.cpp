#include <color/Argb.hpp>
#include <gl/glad.h>
#include <gl/ShaderPrograms.hpp>
#include <gui/DrawContext.hpp>
#include <text/TextRenderer.hpp>
#include <util/math/MathUtils.hpp>
#include <util/Logger.hpp>
#include <util/PathUtil.hpp>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <memory>

namespace blaze::lightEngine {

DrawContext::DrawContext(Window* window): window(window), path(), lineWidth(1), layer(0), debugTest(0) {
    //this->scissorStack = new ScissorStack();
}

void DrawContext::init() {
    GLuint vao;
    glGenVertexArrays(1, &vao);
    this->vao = vao;

    GLuint vbo;
    glGenBuffers(1, &vbo);
    this->vbo = vbo;

    this->setupShaderProgram();

    try {
        this->textRenderer = std::make_unique<TextRenderer>(PathUtil::resolveResource("fonts/segoe.ttf"), 16, true);
    } catch (std::exception ex) {
        LOG_ERROR(ex.what());
        throw ex;
    }
    this->debugTextRenderer = std::make_unique<TextRenderer>(PathUtil::resolveResource("fonts/monospace.ttf"), 20, true);
}

void DrawContext::beginPath() {
    this->path = ShapePath();
}

ShapePath DrawContext::moveTo(float x, float y) {
    return this->path.moveTo(x, y);
}

ShapePath DrawContext::lineTo(float x, float y) {
    return this->path.lineTo(x, y);
}

void DrawContext::endPath() {
    this->path.endPath();
}

Window* DrawContext::getWindow() {
    return this->window;
}

void DrawContext::stroke(int argb) {
    this->stroke(Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb));
}

void DrawContext::stroke(int r, int g, int b, int a) {
    this->shader->bind();
    this->shader->setUniform("uColor", (float) r / 255, (float) g / 255, (float) b / 255, (float) a / 255);
    glBindVertexArray(this->vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

    this->specifyVertexAttributes();

    glBufferData(GL_ARRAY_BUFFER, this->path.getBufferSize(), this->path.vertexData(), GL_STREAM_DRAW);
    glDrawArrays(GL_LINE_LOOP, 0, this->path.getVertexCount());

    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    this->endPath();
    this->shader->unbind();
}

void DrawContext::fill(int argb) {
    this->fill(Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb));
}

void DrawContext::fill(int r, int g, int b, int a) {
    this->shader->bind();
    this->shader->setUniform("uColor", (float) r / 255, (float) g / 255, (float) b / 255, (float) a / 255);

    glEnable(GL_BLEND);
    glDepthFunc(GL_LEQUAL);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(this->vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

    this->specifyVertexAttributes();

    glBufferData(GL_ARRAY_BUFFER, this->path.getBufferSize(), this->path.vertexData(), GL_STREAM_DRAW);
    glDrawArrays(GL_TRIANGLE_FAN, 0, this->path.getVertexCount());


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    this->endPath();
    this->shader->unbind();
}

float DrawContext::getLineWidth() {
    return this->lineWidth;
}

int DrawContext::getLayer() {
    return this->layer;
}

void DrawContext::setLineWidth(float lineWidth) {
    this->lineWidth = lineWidth;
    glLineWidth(lineWidth);
}

void DrawContext::setLayer(int layer) {
    this->layer = layer;
    float depth = 1.0f - (layer * 0.1f);
    glDepthRange(depth, depth);
}

void DrawContext::setTransparent(bool transparent) {
    if (transparent) glDepthMask(GL_FALSE);
    else glDepthMask(GL_TRUE);
}

void DrawContext::drawHorizontalLine(float x1, float x2, float y, int argb) {
    this->drawHorizontalLine(x1, x2, y, Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb));
}

void DrawContext::drawHorizontalLine(float x1, float x2, float y, int r, int g, int b, int a) {
    this->drawLine(x1, x2, y, y, r, g, b, a);
}

void DrawContext::drawVerticalLine(float x, float y1, float y2, int argb) {
    this->drawVerticalLine(x, y1, y2, Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb));
}

void DrawContext::drawVerticalLine(float x, float y1, float y2, int r, int g, int b, int a) {
    this->drawLine(x, x, y1, y2, r, g, b, a);
}

void DrawContext::drawLine(float x1, float x2, float y1, float y2, int argb) {
    this->drawLine(x1, x2, y1, y2, Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb));
}

void DrawContext::drawLine(float x1, float x2, float y1, float y2, int r, int g, int b, int a) {
    this->beginPath();
    this->moveTo(x1, y1);
    this->lineTo(x2, y2);
    this->stroke(r, g, b, a);
}

void DrawContext::drawTextureRegion(Texture& texture, float x, float y, int regX, int regY, int regWidth, int regHeight, int argb, bool flipped) {
    this->drawTextureRegion(texture, x, y, regX, regY, regWidth, regHeight, Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb), flipped);
}

void DrawContext::drawTextureRegion(Texture& texture, float x, float y, int regX, int regY, int regWidth, int regHeight, int r, int g, int b, int a, bool flipped) {
    float x2 = x + regWidth;
    float y2 = y + regHeight;

    float s1 = (float) regX / texture.getWidth();
    float t1 = (float) regY / texture.getHeight();
    float s2 = (float) (regX + regWidth) / texture.getWidth();
    float t2 = (float) (regY + regHeight) / texture.getHeight();

    if (flipped) this->drawTextureRegion(texture, x, y, x2, y2, s1, t1, s2, t2, r, g, b, a);
    else this->drawTextureRegion(texture, x, y, x2, y2, s1, (1.0f - t2), s2, (1.0f - t1), r, g, b, a);
}

void DrawContext::drawTextureRegion(Texture& texture ,float x1, float y1, float x2, float y2, float s1, float t1, float s2, float t2, int argb) {
    this->drawTextureRegion(texture, x1, y1, x2, y2, s1, t1, s2, t2, Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb));
}

void DrawContext::drawTextureRegion(Texture& texture, float x1, float y1, float x2, float y2, float s1, float t1, float s2, float t2, int r, int g, int b, int a) {
    this->textureShader->bind();
    this->textureShader->setUniform("uTexture", 0);
    this->textureShader->setUniform("uColor", (float) r / 255, (float) g / 255, (float) b / 255, (float) a / 255);
    texture.bind();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(this->vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

    std::vector<float> vertices = {
        x1, y1, s1, t2,
        x2, y1, s2, t2,
        x2, y2, s2, t1,

        x2, y2, s2, t1,
        x1, y1, s1, t2,
        x1, y2, s1, t1
    };

    this->specifyTextureAttributes();

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    texture.unbind();
    this->textureShader->unbind();
}

void DrawContext::drawTextureBatch(Texture& texture, const std::vector<float>& vertices, int argb) {
    this->drawTextureBatch(texture, vertices, Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb));
}

void DrawContext::drawTextureBatch(Texture& texture, const std::vector<float>& vertices, int r, int g, int b, int a) {
    if (vertices.empty()) return;

    this->textureShader->bind();
    this->textureShader->setUniform("uTexture", 0);
    this->textureShader->setUniform("uColor", (float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, (float)a / 255.0f);
    texture.bind();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(this->vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

    this->specifyTextureAttributes();

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    GLsizei totalVertices = static_cast<GLsizei>(vertices.size() / 4);
    glDrawArrays(GL_TRIANGLES, 0, totalVertices);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    texture.unbind();
    this->textureShader->unbind();
}

void DrawContext::drawTexture(Texture& texture, float x, float y, int argb) {
    this->drawTexture(texture, x, y, 0, 0, 1, 1, argb);
}

void DrawContext::drawTexture(Texture& texture, float x, float y, float width, float height, int argb) {
    this->drawTexture(texture, x, y, width, height, 0, 0, 1, 1, argb);
}

void DrawContext::drawTexture(Texture& texture, float x, float y, float u0, float v0, float u1, float v1, int argb) {
    this->drawTexture(texture, x, y, texture.getWidth(), texture.getHeight(), u0, v0, u1, v1, Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb));
}

void DrawContext::drawTexture(Texture& texture, float x, float y, float u0, float v0, float u1, float v1, int r, int g, int b, int a) {
    this->drawTexture(texture, x, y, texture.getWidth(), texture.getHeight(), u0, v0, u1, v1, r, g, b, a);
}

void DrawContext::drawTexture(Texture& texture, float x, float y, float width, float height, float u0, float v0, float u1, float v1, int argb) {
    this->drawTexture(texture, x, y, width, height, u0, v0, u1, v1, Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb));
}

void DrawContext::drawTexture(Texture& texture, float x, float y, float width, float height, float u0, float v0, float u1, float v1, int r, int g, int b, int a) {
    this->textureShader->bind();
    this->textureShader->setUniform("uTexture", 0);
    this->textureShader->setUniform("uColor", (float) r / 255, (float) g / 255, (float) b / 255, (float) a / 255);

    texture.bind();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(this->vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

    std::vector<float> buffer =  {
        x, y, u0, v1,
        x + width, y, u1, v1,
        x + width, y + height, u1, v0,

        x + width, y + height, u1, v0,
        x, y, u0, v1,
        x, y + height, u0, v0
    };

    this->specifyTextureAttributes();

    glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(float), buffer.data(), GL_DYNAMIC_DRAW);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    texture.unbind();
    this->textureShader->unbind();
}

void DrawContext::drawQuadWithBorder(float x, float y, float width, float height, int argb, float spacing, float stroke, int argbStroke) {
    this->drawQuadWithBorder(x, y, width, height, Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb), spacing, stroke, Argb::getRed(argbStroke), Argb::getGreen(argbStroke), Argb::getBlue(argbStroke), Argb::getAlpha(argbStroke));
}

void DrawContext::drawQuad(float x, float y, float width, float height, int argb) {
    this->drawQuad(x, y, width, height, Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb));
}

void DrawContext::drawQuadWithBorder(float x, float y, float width, float height, int r, int g, int b, int a, float spacing, float stroke, int rStroke, int gStroke, int bStroke, int aStroke) {
    this->drawQuad(x, y, width, height, r, g, b, a);
    this->drawBorder(x, y, width, height, rStroke, gStroke, bStroke, aStroke, spacing, stroke);
}

void DrawContext::drawQuad(float x, float y, float width, float height, int r, int g, int b, int a) {
    this->beginPath();
    this->moveTo(x, y);
    this->lineTo(x + width, y);
    this->lineTo(x + width, y + height);
    this->lineTo(x, y + height);
    this->lineTo(x, y);
    this->fill(r, g, b, a);
}

void DrawContext::drawBorder(float x, float y, float width, float height, int argb, float spacing, float stroke) {
    this->drawBorder(x, y, width, height, Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb), spacing, stroke);
}

void DrawContext::drawBorder(float x, float y, float width, float height, int r, int g, int b, int a, float spacing, float stroke) {
    float hStroke = stroke / 2;
    float x1 = x - spacing - hStroke;
    float x2 = x + width + spacing + hStroke;
    float y1 = y - spacing - hStroke;
    float y2 = y + height + spacing + hStroke;

    this->setLineWidth(stroke);
    this->drawHorizontalLine(x1, x2, y1, r, g, b, a);
    this->drawHorizontalLine(x1, x2, y2, r, g, b, a);
    this->drawVerticalLine(x1, y1, y2, r, g, b, a);
    this->drawVerticalLine(x2, y1, y2, r, g, b, a);
}

void DrawContext::drawCircleWithBorder(float cx, float cy, float radius, int segments, int argb, bool fill, float spacing, float stroke, int argbStroke) {
    this->drawCircleWithBorder(cx, cy, radius, segments, Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb), fill, spacing, stroke, Argb::getRed(argbStroke), Argb::getGreen(argbStroke), Argb::getBlue(argbStroke), Argb::getAlpha(argbStroke));
}

void DrawContext::drawCircle(float cx, float cy, float radius, int segments, int argb, bool fill) {
    this->drawCircle(cx, cy, radius, segments, Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb), fill);
}

void DrawContext::drawCircleWithBorder(float cx, float cy, float radius, int segments, int r, int g, int b, int a, bool fill, float spacing, float stroke, int rStroke, int gStroke, int bStroke, int aStroke) {
    this->setLineWidth(stroke);
    this->drawCircle(cx, cy, radius + spacing, segments, rStroke, gStroke, bStroke, aStroke, false);
    this->drawCircle(cx, cy, radius, segments, r, g, b, a, fill);
}

void DrawContext::drawCircle(float cx, float cy, float radius, int segments, int r, int g, int b, int a, bool fill) {
    this->shader->bind();
    this->shader->setUniform("uColor", (float) r / 255, (float) g / 255, (float) b / 255, (float) a / 255);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(this->vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

    this->specifyVertexAttributes();

    this->beginPath();
    if (fill) this->moveTo(cx, cy);

    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * PI * float(i) / float(segments);
        float px = cx + std::cos(angle) * radius;
        float py = cy + std::sin(angle) * radius;
        this->lineTo(px, py);
    }

    glBufferData(GL_ARRAY_BUFFER, this->path.getBufferSize(), this->path.vertexData(), GL_STATIC_DRAW);

    if (fill) glDrawArrays(GL_TRIANGLE_FAN, 0, this->path.getVertexCount());
    else glDrawArrays(GL_LINE_LOOP, 0, this->path.getVertexCount());

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    this->endPath();
    this->shader->unbind();
}

void DrawContext::drawText(float x, float y, std::string text, int r, int g, int b, int a) {
    if (text.empty()) return;
    this->textRenderer->drawText(this, text, x, y, r, g, b, a);
}

void DrawContext::drawText(float x, float y, std::string text, int color) {
    this->drawText(x, y, text, Argb::getRed(color), Argb::getGreen(color), Argb::getBlue(color), Argb::getAlpha(color));
}

void DrawContext::drawText(float x, float y, std::string text) {
    this->drawText(x, y, text, 0xFFFFFFFF);
}

void DrawContext::drawCenteredText(float x, float y, std::string text) {
    this->drawCenteredText(x, y, text, 0xFFFFFFFF);
}

void DrawContext::drawCenteredText(float x, float y, std::string text, int argb) {
    this->drawCenteredText(x, y, text, Argb::getRed(argb) , Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb));
}

void DrawContext::drawCenteredText(float x, float y, std::string text, int r, int g, int b, int a) {
    this->drawText(x - (float) this->textRenderer->getWidth(text) / 2, y, text, r, g, b, a);
}

void DrawContext::drawTextWithShadow(float x, float y, std::string text, int argb) {
    this->drawTextWithShadow(x, y, text, Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb));
}

void DrawContext::drawTextWithShadow(float x, float y, std::string text, int r, int g, int b, int a) {
    this->textRenderer->drawText(this, text, x, y, r, g, b, a, true);
}

void DrawContext::drawCenteredTextWithShadow(float x, float y, std::string text, int argb) {
    this->drawCenteredTextWithShadow(x, y, text, Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb));
}

void DrawContext::drawCenteredTextWithShadow(float x, float y, std::string text, int r, int g, int b, int a) {
    if (text.empty()) return;
    this->textRenderer->drawText(this, text, x - (float) this->textRenderer->getWidth(text) / 2, y, r, g, b, a, true);
}

float DrawContext::getTextWidth(std::string text) {
    return this->textRenderer->getWidth(text);
}

float DrawContext::getTextHeight(std::string text) {
    return this->textRenderer->getHeight(text);
}

void DrawContext::drawDebugText(float x, float y, std::string text, int r, int g, int b, int a) {
    if (text.empty()) return;
    this->debugTextRenderer->drawText(this, text, x, y, r, g, b, a);
}

void DrawContext::drawDebugText(float x, float y, std::string text, int argb) {
    this->drawDebugText(x, y, text, Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb));
}

void DrawContext::drawDebugText(float x, float y, std::string text) {
    this->drawDebugText(x, y, text, 0xFFFFFFFF);
}

void DrawContext::drawCenteredDebugText(float x, float y, std::string text) {
    this->drawCenteredDebugText(x, y, text, 0xFFFFFFFF);
}

void DrawContext::drawCenteredDebugText(float x, float y, std::string text, int argb) {
    this->drawCenteredDebugText(x, y, text, Argb::getRed(argb) , Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb));
}

void DrawContext::drawCenteredDebugText(float x, float y, std::string text, int r, int g, int b, int a) {
    this->drawDebugText(x - (float) this->debugTextRenderer->getWidth(text) / 2, y, text, r, g, b, a);
}

void DrawContext::drawDebugTextWithShadow(float x, float y, std::string text, int argb) {
    this->drawDebugTextWithShadow(x, y, text, Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb));
}

void DrawContext::drawDebugTextWithShadow(float x, float y, std::string text, int r, int g, int b, int a) {
    this->debugTextRenderer->drawText(this, text, x, y, r, g, b, a, true);
}

void DrawContext::drawCenteredDebugTextWithShadow(float x, float y, std::string text, int argb) {
    this->drawCenteredDebugTextWithShadow(x, y, text, Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb));
}

void DrawContext::drawCenteredDebugTextWithShadow(float x, float y, std::string text, int r, int g, int b, int a) {
    if (text.empty()) return;
    this->debugTextRenderer->drawText(this, text, x - (float) this->debugTextRenderer->getWidth(text) / 2, y, r, g, b, a, true);
}

float DrawContext::getDebugTextWidth(std::string text) {
    return this->debugTextRenderer->getWidth(text);
}

float DrawContext::getDebugTextHeight(std::string text) {
    return this->debugTextRenderer->getHeight(text);
}

void DrawContext::specifyTextureAttributes() {
    this->textureShader->enableVertexAttribute(0);
    this->textureShader->pointVertexAttribute(0, 2, 4 * sizeof(float),(void*)0);

    this->textureShader->enableVertexAttribute(1);
    this->textureShader->pointVertexAttribute(1, 2, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void DrawContext::specifyVertexAttributes() {
    this->shader->enableVertexAttribute(0);
    this->shader->pointVertexAttribute(0, 2, 2 * sizeof(float), 0);
}

void DrawContext::setupShaderProgram() {
    glBindVertexArray(this->vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    this->shader = std::move(ShaderPrograms::POSITION);
    this->textureShader = std::move(ShaderPrograms::POSITION_TEX);

    int width, height;
    glfwGetFramebufferSize(this->window->getHandle(), &width, &height);

    glm::mat4 proj = glm::ortho(0, width, height, 0, -1, 1);

    // Ensure both shaders have the projection uniform set
    if (this->shader) {
        this->shader->bind();
        this->shader->setUniform("uProjection", proj);
        // Log attribute/uniform locations for debugging
        int posLoc = this->shader->getAttributeLocation("aPos");
        int colorLoc = this->shader->getUniformLocation("uColor");
        int projLoc = this->shader->getUniformLocation("uProjection");
        LOG_DEBUG(std::string("Shader POSITION - aPos=") + std::to_string(posLoc) + std::string(" uColor=") + std::to_string(colorLoc) + std::string(" uProjection=") + std::to_string(projLoc));
        this->shader->unbind();
    }

    if (this->textureShader) {
        this->textureShader->bind();
        this->textureShader->setUniform("uProjection", proj);
        // Set default texture unit for texture shader if present
        this->textureShader->setUniform("uTexture", 0);
        int posLocT = this->textureShader->getAttributeLocation("aPos");
        int uvLocT = this->textureShader->getAttributeLocation("aUv");
        int texLoc = this->textureShader->getUniformLocation("uTexture");
        LOG_DEBUG(std::string("Shader POSITION_TEX - aPos=") + std::to_string(posLocT) + std::string(" aUv=") + std::to_string(uvLocT) + std::string(" uTexture=") + std::to_string(texLoc));
        this->textureShader->unbind();
    }
}

void DrawContext::setProjection(int width, int height) {
    glm::mat4 proj = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1.0f);

    this->shader->bind();
    this->shader->setUniform("uProjection", proj);
    this->shader->unbind();

    this->textureShader->bind();
    this->textureShader->setUniform("uProjection", proj);
    this->textureShader->unbind();
}

}

