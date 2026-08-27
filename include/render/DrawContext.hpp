#pragma once

#include <gl/ShaderProgram.hpp>
#include <render/ShapePath.hpp>
#include <text/TextRenderer.hpp>
#include <texture/Texture.hpp>
#include <util/Window.hpp>

#include <memory>
#include <string>
#include <vector>

namespace blaze::lightEngine {

class DrawContext {
public:
	DrawContext(Window* window);

	std::shared_ptr<TextRenderer> textRenderer = nullptr;
	std::shared_ptr<TextRenderer> debugTextRenderer = nullptr;

	void init();
	void beginPath();
	ShapePath moveTo(float x, float y);
	ShapePath lineTo(float x, float y);
	void endPath();
	Window* getWindow();
	void stroke(int argb) { this->stroke(Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb)); };
	void strole(Color& color) { this->stroke(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha()); };
	void stroke(int r, int g, int b, int a);
	void fill(int argb) { this->fill(Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb)); }
	void fill(Color& color) { this->fill(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha()); };
	void fill(int r, int g, int b, int a);
	float getLineWidth();
	int getLayer();
	void setLineWidth(float lineWidth);
	void setLayer(int layer);
	void setTransparent(bool transparent);
	void loadFont(const std::string& path, int size = 16, bool antiAlias = true);
	void loadDebugFont(const std::string& path, int size = 16, bool antiAlias = true);
	void setTextRenderer(std::shared_ptr<TextRenderer> textRenderer);
	void setDebugTextRenderer(std::shared_ptr<TextRenderer> debugTextRenderer);
	
	void drawHorizontalLine(float x1, float x2, float y, int argb) { this->drawHorizontalLine(x1, x2, y, Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb)); }
	void drawHorizontalLine(float x1, float x2, float y, Color& color) { this->drawHorizontalLine(x1, x2, y, color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha()); };
	void drawHorizontalLine(float x1, float x2, float y, int r, int g, int b, int a);
	void drawVerticalLine(float x, float y1, float y2, int argb)  { this->drawVerticalLine(x, y1, y2, Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb)); };
	void drawVerticalLine(float x, float y1, float y2, Color& color) { this->drawVerticalLine(x, y1, y2, color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha()); }
	void drawVerticalLine(float x, float y1, float y2, int r, int g, int b, int a);
	void drawLine(float x1, float x2, float y1, float y2, int argb) { this->drawLine(x1, x2, y1, y2, Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb)); }
	void drawLine(float x1, float x2, float y1, float y2, Color& color) { this->drawLine(x1, x2, y1, y2, color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha()); };
	void drawLine(float x1, float x2, float y1, float y2, int r, int g, int b, int a);
	
	void drawTextureRegion(Texture& texture, float x, float y, int regX, int regY, int regWidth, int regHeight, int argb, bool flipped = false) { this->drawTextureRegion(texture, x, y, regX, regY, regWidth, regHeight, Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb), flipped); }
	void drawTextureRegion(Texture& texture, float x, float y, int regX, int regY, int regWidth, int regHeight, Color& color, bool flipped = false) { this->drawTextureRegion(texture, x, y, regX, regY, regWidth, regHeight, color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha(), flipped); }
	void drawTextureRegion(Texture& texture, float x, float y, int regX, int regY, int regWidth, int regHeight, int r, int g, int b, int a, bool flipped = false);
	void drawTextureRegion(Texture& texture, float x1, float y1, float x2, float y2, float s1, float t1, float s2, float t2, int argb) { this->drawTextureRegion(texture, x1, y1, x2, y2, s1, t1, s2, t2, Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb)); }
	void drawTextureRegion(Texture& texture, float x1, float y1, float x2, float y2, float s1, float t1, float s2, float t2, Color& color) { this->drawTextureRegion(texture, x1, y1, x2, y2, s1, t1, s1, t1, color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha()); }
	void drawTextureRegion(Texture& texture, float x1, float y1, float x2, float y2, float s1, float t1, float s2, float t2, int r, int g, int b, int a);

	void drawTextureBatch(Texture& texture, const std::vector<float>& vertices, int argb);
	void drawTextureBatch(Texture& texture, const std::vector<float>& vertices, Color& color) { this->drawTextureBatch(texture, vertices, color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha()); };
	void drawTextureBatch(Texture& texture, const std::vector<float>& vertices, int r, int g, int b, int a);
	void drawTextureBatch(Texture& texture, const float* vertices, size_t count, int argb) { this->drawTextureBatch(texture, vertices, count, Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb)); }
	void drawTextureBatch(Texture& texture, const float* vertices, size_t count, Color& color) { this->drawTextureBatch(texture, vertices, count, color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha()); }
	void drawTextureBatch(Texture& texture, const float* vertices, size_t count, int r, int g, int b, int a);
	void flushTextureBatch();

	void drawTexture(Texture& texture, float x, float y, int argb) { this->drawTexture(texture, x, y, 0, 0, 1, 1, argb); }
	void drawTexture(Texture& texture, float x, float y, Color& color) { this->drawTexture(texture, x, y, 0, 0, 1, 1, color); };
	void drawTexture(Texture& texture, float x, float y, float width, float height, int argb) { this->drawTexture(texture, x, y, width, height, 0, 0, 1, 1, argb); };
	void drawTexture(Texture& texture, float x, float y, float width, float height, Color& color) { this->drawTexture(texture, x, y, width, height, 0, 0, 1, 1, color); }
	void drawTexture(Texture& texture, float x, float y, float u0, float v0, float u1, float v1, int argb) { this->drawTexture(texture, x, y, texture.getWidth(), texture.getHeight(), u0, v0, u1, v1, Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb)); }
	void drawTexture(Texture& texture, float x, float y, float u0, float v0, float u1, float v1, Color& color) { this->drawTexture(texture, x, y, u0, u1, v0, v1, color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha()); }
	void drawTexture(Texture& texture, float x, float y, float u0, float v0, float u1, float v1, int r, int g, int b, int a);
	void drawTexture(Texture& texture, float x, float y, float width, float height, float u0, float v0, float u1, float v1, int argb) { this->drawTexture(texture, x, y, width, height, u0, v0, u1, v1, Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb)); }
	void drawTexture(Texture& texture, float x, float y, float width, float height, float u0, float v0, float u1, float v1, Color& color) { this->drawTexture(texture, x, y, width, height, u0, v0, u1, v1, color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha()); }
	void drawTexture(Texture& texture, float x, float y, float width, float height, float u0, float v0, float u1, float v1, int r, int g, int b, int a);

	void drawQuadWithBorder(float x, float y, float width, float height, int argb, float spacing, float stroke, int argbStroke) { this->drawQuadWithBorder(x, y, width, height, Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb), spacing, stroke, Argb::getRed(argbStroke), Argb::getGreen(argbStroke), Argb::getBlue(argbStroke), Argb::getAlpha(argbStroke)); };
	void drawQuadWithBorder(float x, float y, float width, float height, Color& color, float spacing, float stroke, Color& colorStroke) { this->drawQuadWithBorder(x, y, width, height, color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha(), spacing, stroke, colorStroke.getRed(), colorStroke.getGreen(), colorStroke.getBlue(), colorStroke.getAlpha()); }
	void drawQuad(float x, float y, float width, float height, int argb) { this->drawQuad(x, y, width, height, Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb)); }
	void drawQuad(float x, float y, float width, float height, Color& color) { this->drawQuad(x, y, width, height, color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha()); }
	void drawQuadWithBorder(float x, float y, float width, float height, int r, int g, int b, int a, float spacing, float stroke, int rStroke, int gStroke, int bStroke, int aStroke);
	void drawQuad(float x, float y, float width, float height, int r, int g, int b, int a);
	void drawBorder(float x, float y, float width, float height, int argb, float spacing, float stroke) { this->drawBorder(x, y, width, height, Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb), spacing, stroke); }
	void drawBorder(float x, float y, float width, float height, Color& color, float spacing, float stroke) { this->drawBorder(x, y, width, height, color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha(), spacing, stroke); }
	void drawBorder(float x, float y, float width, float height, int r, int g, int b, int a, float spacing, float stroke);

	void drawCircleWithBorder(float cx, float cy, float radius, int segments, int argb, bool fill, float spacing, float stroke, int argbStroke) { this->drawCircleWithBorder(cx, cy, radius, segments, Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb), fill, spacing, stroke, Argb::getRed(argbStroke), Argb::getGreen(argbStroke), Argb::getBlue(argbStroke), Argb::getAlpha(argbStroke)); }
	void drawCircleWithBorder(float cx, float cy, float radius, int segments, Color color, bool fill, float spacing, float stroke, Color colorStroke) { this->drawCircleWithBorder(cx, cy, radius, segments, color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha(), fill, spacing, stroke, colorStroke.getRed(), colorStroke.getGreen(),colorStroke.getBlue(), colorStroke.getAlpha()); }
	void drawCircle(float cx, float cy, float radius, int segments, int argb, bool fill) { this->drawCircle(cx, cy, radius, segments, Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb), fill); }
	void drawCircle(float cx, float cy, float radius, int segments, Color color, bool fill) { this->drawCircle(cx, cy, radius, segments, color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha(), fill); }
	void drawCircleWithBorder(float cx, float cy, float radius, int segments, int r, int g, int b, int a, bool fill, float spacing, float stroke, int rStroke, int gStroke, int bStroke, int aStroke);
	void drawCircle(float cx, float cy, float radius, int segments, int r, int g, int b, int a, bool fill);

	void drawText(float x, float y, const std::string& text) { this->drawText(x, y, text, Color::WHITE); };
	void drawText(float x, float y, const std::string& text, int argb) { this->drawText(x, y, text, Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb)); }
	void drawText(float x, float y, const std::string& text, Color& color) { this->drawText(x, y, text, color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha()); };
	void drawText(float x, float y, const std::string& text, int r, int g, int b, int a);
	void drawCenteredText(float x, float y, const std::string& text) { this->drawCenteredText(x, y, text, Color::WHITE); }
	void drawCenteredText(float x, float y, const std::string& text, int argb) { this->drawCenteredText(x, y, text, Argb::getRed(argb) , Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb)); }
	void drawCenteredText(float x, float y, const std::string& text, Color& color) { this->drawCenteredText(x, y, text, color.getRed() , color.getGreen(), color.getBlue(), color.getAlpha()); }
	void drawCenteredText(float x, float y, const std::string& text, int r, int g, int b, int a);
	void drawTextWithShadow(float x, float y, const std::string& text, int argb) { this->drawTextWithShadow(x, y, text, Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb)); }
	void drawTextWithShadow(float x, float y, const std::string& text, Color& color) { this->drawTextWithShadow(x, y, text, color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha()); }
	void drawTextWithShadow(float x, float y, const std::string& text, int r, int g, int b, int a);
	void drawCenteredTextWithShadow(float x, float y, const std::string& text, int argb) { this->drawCenteredTextWithShadow(x, y, text, Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb)); }
	void drawCenteredTextWithShadow(float x, float y, const std::string& text, Color& color) { this->drawCenteredTextWithShadow(x, y, text, color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha()); }
	void drawCenteredTextWithShadow(float x, float y, const std::string& text, int r, int g, int b, int a);
	float getTextWidth(std::string text);
	float getTextHeight(std::string text);

	void drawDebugText(float x, float y, const std::string& text) { this->drawDebugText(x, y, text, Color::WHITE); };
	void drawDebugText(float x, float y, const std::string& text, int argb) { this->drawDebugText(x, y, text, Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb)); }
	void drawDebugText(float x, float y, const std::string& text, Color& color) { this->drawDebugText(x, y, text, color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha()); };
	void drawDebugText(float x, float y, const std::string& text, int r, int g, int b, int a);
	void drawCenteredDebugText(float x, float y, const std::string& text) { this->drawCenteredDebugText(x, y, text, Color::WHITE); }
	void drawCenteredDebugText(float x, float y, const std::string& text, int argb) { this->drawCenteredDebugText(x, y, text, Argb::getRed(argb) , Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb)); }
	void drawCenteredDebugText(float x, float y, const std::string& text, Color& color) { this->drawCenteredDebugText(x, y, text, color.getRed() , color.getGreen(), color.getBlue(), color.getAlpha()); }
	void drawCenteredDebugText(float x, float y, const std::string& text, int r, int g, int b, int a);
	void drawDebugTextWithShadow(float x, float y, const std::string& text, int argb) { this->drawDebugTextWithShadow(x, y, text, Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb)); }
	void drawDebugTextWithShadow(float x, float y, const std::string& text, Color& color) { this->drawDebugTextWithShadow(x, y, text, color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha()); }
	void drawDebugTextWithShadow(float x, float y, const std::string& text, int r, int g, int b, int a);
	void drawCenteredDebugTextWithShadow(float x, float y, const std::string& text, int argb) { this->drawCenteredDebugTextWithShadow(x, y, text, Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb)); }
	void drawCenteredDebugTextWithShadow(float x, float y, const std::string& text, Color& color) { this->drawCenteredDebugTextWithShadow(x, y, text, color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha()); }
	void drawCenteredDebugTextWithShadow(float x, float y, const std::string& text, int r, int g, int b, int a);
	float getDebugTextWidth(std::string text);
	float getDebugTextHeight(std::string text);
	void setProjection(int width, int height);

private:
	int vao;
	int textureVao;
	int vbo;
	int debugTest = 0;
	int layer;
	float lineWidth;
	std::vector<float> textureVertices;
	GLuint textureBatchTextureId = 0;
	int textureBatchR = 255;
	int textureBatchG = 255;
	int textureBatchB = 255;
	int textureBatchA = 255;
	std::shared_ptr<ShaderProgram> shader;
	std::shared_ptr<ShaderProgram> textureShader;
	ShapePath path;
	Window* window;

	void specifyTextureAttributes();
	void specifyVertexAttributes();
	void setupShaderProgram();
};

}
