#pragma once

#include "../gl/ShaderProgram.hpp"
#include "../render/ShapePath.hpp"
#include "../text/TextRenderer.hpp"
#include "../texture/Texture.hpp"
#include "../util/Window.hpp"

#include <memory>
#include <string>

namespace blaze::lightEngine {

class DrawContext {
public:
    DrawContext(Window* window);

    std::unique_ptr<TextRenderer> textRenderer;
    std::unique_ptr<TextRenderer> debugTextRenderer;

    void init();
    void beginPath();
    ShapePath moveTo(float x, float y);
    ShapePath lineTo(float x, float y);
    void endPath();
    Window* getWindow();
    void stroke(int argb);
    void stroke(int r, int g, int b, int a);
    void fill(int argb);
    void fill(int r, int g, int b, int a);
    float getLineWidth();
    int getLayer();
    void setLineWidth(float lineWidth);
    void setLayer(int layer);
    void setTransparent(bool transparent);

    void drawHorizontalLine(float x1, float x2, float y, int argb);
    void drawHorizontalLine(float x1, float x2, float y, int r, int g, int b, int a);
    void drawVerticalLine(float x, float y1, float y2, int argb);
    void drawVerticalLine(float x, float y1, float y2, int r, int g, int b, int a);
    void drawLine(float x1, float x2, float y1, float y2, int argb);
    void drawLine(float x1, float x2, float y1, float y2, int r, int g, int b, int a);

    void drawTextureRegion(Texture& texture, float x, float y, int regX, int regY, int regWidth, int regHeight, int argb, bool flipped = false);
    void drawTextureRegion(Texture& texture, float x, float y, int regX, int regY, int regWidth, int regHeight, int r, int g, int b, int a, bool flipped = false);
    void drawTextureRegion(Texture& texture, float x1, float y1, float x2, float y2, float s1, float t1, float s2, float t2, int argb);
    void drawTextureRegion(Texture& texture, float x1, float y1, float x2, float y2, float s1, float t1, float s2, float t2, int r, int g, int b, int a);

    void drawTextureBatch(Texture& texture, const std::vector<float>& vertices, int argb);
    void drawTextureBatch(Texture& texture, const std::vector<float>& vertices, int r, int g, int b, int a);

    void drawTexture(Texture& texture, float x, float y, int argb);
    void drawTexture(Texture& texture, float x, float y, float width, float height, int argb);
    void drawTexture(Texture& texture, float x, float y, float u0, float v0, float u1, float v1, int argb);
    void drawTexture(Texture& texture, float x, float y, float u0, float v0, float u1, float v1, int r, int g, int b, int a);
    void drawTexture(Texture& texture, float x, float y, float width, float height, float u0, float v0, float u1, float v1, int argb);
    void drawTexture(Texture& texture, float x, float y, float width, float height, float u0, float v0, float u1, float v1, int r, int g, int b, int a);

    void drawQuadWithBorder(float x, float y, float width, float height, int argb, float spacing, float stroke, int argbStroke);
    void drawQuad(float x, float y, float width, float height, int argb);
    void drawQuadWithBorder(float x, float y, float width, float height, int r, int g, int b, int a, float spacing, float stroke, int rStroke, int gStroke, int bStroke, int aStroke);
    void drawQuad(float x, float y, float width, float height, int r, int g, int b, int a);
    void drawBorder(float x, float y, float width, float height, int argb, float spacing, float stroke);
    void drawBorder(float x, float y, float width, float height, int r, int g, int b, int a, float spacing, float stroke);

    void drawCircleWithBorder(float cx, float cy, float radius, int segments, int argb, bool fill, float spacing, float stroke, int argbStroke);
    void drawCircle(float cx, float cy, float radius, int segments, int argb, bool fill);
    void drawCircleWithBorder(float cx, float cy, float radius, int segments, int r, int g, int b, int a, bool fill, float spacing, float stroke, int rStroke, int gStroke, int bStroke, int aStroke);
    void drawCircle(float cx, float cy, float radius, int segments, int r, int g, int b, int a, bool fill);

    void drawText(float x, float y, std::string text, int r, int g, int b, int a);
    void drawText(float x, float y, std::string text, int color);
    void drawText(float x, float y, std::string text);
    void drawCenteredText(float x, float y, std::string text);
    void drawCenteredText(float x, float y, std::string text, int argb);
    void drawCenteredText(float x, float y, std::string text, int r, int g, int b, int a);
    void drawTextWithShadow(float x, float y, std::string text, int argb);
    void drawTextWithShadow(float x, float y, std::string text, int r, int g, int b, int a);
    void drawCenteredTextWithShadow(float x, float y, std::string text, int argb);
    void drawCenteredTextWithShadow(float x, float y, std::string text, int r, int g, int b, int a);
    float getTextWidth(std::string text);
    float getTextHeight(std::string text);

    void drawDebugText(float x, float y, std::string text, int r, int g, int b, int a);
    void drawDebugText(float x, float y, std::string text, int color);
    void drawDebugText(float x, float y, std::string text);
    void drawCenteredDebugText(float x, float y, std::string text);
    void drawCenteredDebugText(float x, float y, std::string text, int argb);
    void drawCenteredDebugText(float x, float y, std::string text, int r, int g, int b, int a);
    void drawDebugTextWithShadow(float x, float y, std::string text, int argb);
    void drawDebugTextWithShadow(float x, float y, std::string text, int r, int g, int b, int a);
    void drawCenteredDebugTextWithShadow(float x, float y, std::string text, int argb);
    void drawCenteredDebugTextWithShadow(float x, float y, std::string text, int r, int g, int b, int a);
    float getDebugTextWidth(std::string text);
    float getDebugTextHeight(std::string text);
    void setProjection(int width, int height);

private:
    int vao;
    int vbo;
    int debugTest = 0;
    int layer;
    float lineWidth;
    // LightEngine* lightEngine;
    std::unique_ptr<ShaderProgram> shader;
    std::unique_ptr<ShaderProgram> textureShader;
    ShapePath path;
    Window* window;

    void specifyTextureAttributes();
    void specifyVertexAttributes();
    void setupShaderProgram();
};

}
