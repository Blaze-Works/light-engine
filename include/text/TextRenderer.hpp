#pragma once

#include <color/Color.hpp>
#include <texture/Texture.hpp>

#include <map>
#include <memory>
#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace blaze::lightEngine {

struct Glyph {
	int width;
	int height;
	int textureX;
	int textureY;
	int textureLeft;
	int textureTop;
	float advanceX;
};

class DrawContext;

class TextRenderer {
public:
	TextRenderer(const std::string& fontPath, int size, bool antiAlias);
	~TextRenderer() = default;
	Texture createFontTexture(FT_Face face, bool antiAlias);
	int getWidth(const std::string& text);
	int getHeight(const std::string& text);
	std::shared_ptr<Texture> getTexture();
	void drawText(DrawContext* context, const std::string& text, float x, float y, int r, int g, int b, int a);
	void drawText(DrawContext* context, const std::string& text, float x, float y, int r, int g, int b, int a, bool hasShadow);
	void drawText(DrawContext* context, const std::string& text, float x, float y, int argb, bool hasShadow = false) { this->drawText(context, text, x, y, Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb)); }
	void drawText(DrawContext* context, const std::string& text, float x, float y, Color color, bool hasShadow = false) { this->drawText(context, text, x, y, color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha(), hasShadow); } 

private:
	int fontHeight;
	int maxAscent;
	std::map<char, Glyph> glyphs;
	std::shared_ptr<Texture> texture;
	void drawTextInternal(DrawContext* context, std::string& text, float x, float y, int r, int g, int b, int a);
};

} // namespace blaze::lightEngine
