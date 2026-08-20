#include <color/Argb.hpp>
#include <render/DrawContext.hpp>
#include <text/TextRenderer.hpp>
#include <texture/Texture.hpp>
#include <texture/TextureUtil.hpp>
#include <util/Logger.hpp>

#include <cstddef>
#include <string>
#include <algorithm>
#include <vector>
#include <sstream>

namespace blaze::lightEngine {

TextRenderer::TextRenderer(const std::string& fontPath, int size, bool antiAlias = true) {
	FT_Library library;
	if (FT_Init_FreeType(&library)) {
		throw std::runtime_error("Failed to initialize FreeType Library");
	}

	FT_Face face;
	if (FT_New_Face(library, fontPath.c_str(), 0, &face)) {
		FT_Done_FreeType(library);
		throw std::runtime_error("Failed to load font file: " + fontPath);
	}

	FT_Set_Pixel_Sizes(face, 0, size);
	this->maxAscent = 0;
	this->texture = std::make_unique<Texture>(this->createFontTexture(face, antiAlias));

	FT_Done_Face(face);
	FT_Done_FreeType(library);
}

Texture TextRenderer::createFontTexture(FT_Face face, bool antiAlias) {
	int imageWidth = 0;
	int imageHeight = 0;

	FT_Render_Mode renderMode = antiAlias ? FT_RENDER_MODE_NORMAL : FT_RENDER_MODE_MONO;

	for (unsigned int i = 32; i < 256; i++) {
		if (i == 127) continue;

		FT_UInt glyph_index = FT_Get_Char_Index(face, i);
		if (FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT) != 0) continue;
		FT_Render_Glyph(face->glyph, renderMode);

		imageWidth += face->glyph->bitmap.width;
		imageHeight = std::max(imageHeight, (int)face->glyph->bitmap.rows);
	}

	this->fontHeight = imageHeight;

	std::vector<unsigned char> textureBuffer(imageWidth * imageHeight, 0);

	int currentX = 0;

	for (unsigned int i = 32; i < 128; i++) {
		if (i == 127) continue;

		FT_UInt glyph_index = FT_Get_Char_Index(face, i);
		if (FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT) != 0) continue;
		FT_Render_Glyph(face->glyph, renderMode);

		FT_Bitmap& bitmap = face->glyph->bitmap;
		int charWidth = bitmap.width;
		int charHeight = bitmap.rows;

		Glyph glyph = {charWidth, charHeight, currentX, 0, face->glyph->bitmap_left, face->glyph->bitmap_top, static_cast<float>(face->glyph->advance.x >> 6)};

		for (int r = 0; r < charHeight; r++) {
			for (int c = 0; c < charWidth; c++) {
				unsigned char alpha = 0;

				if (antiAlias) alpha = bitmap.buffer[r * bitmap.pitch + c];
				else {
					unsigned char byte = bitmap.buffer[r * bitmap.pitch + (c / 8)];
					alpha = (byte & (0x80 >> (c % 8))) ? 255 : 0;
				}

				int flippedRow = (charHeight - 1 - r);
				int targetIndex = (flippedRow * imageWidth) + (currentX + c);

				textureBuffer[targetIndex] = alpha;
			}
		}

		this->maxAscent = std::max(this->maxAscent, static_cast<int>(face->glyph->bitmap_top));
		glyphs[static_cast<unsigned char>(i)] = glyph;
		currentX += charWidth;
	}

	TextureUtil::unpackAlignment();

	return TextureUtil::createAlphaTexture(imageWidth, imageHeight, textureBuffer.data());
}


int TextRenderer::getWidth(const std::string& text) {
	int maxWidth = 0;
	int lineWidth = 0;
	for (char c : text) {
		if (c == '\n') {
			maxWidth = std::max(maxWidth, lineWidth);
			lineWidth = 0;
			continue;
		}
		if (c == '\r') continue;

		auto it = glyphs.find(static_cast<unsigned char>(c));
		if (it != glyphs.end()) {
			lineWidth += static_cast<int>(it->second.advanceX);
		}
	}
	return std::max(maxWidth, lineWidth);
}

int TextRenderer::getHeight(const std::string& text) {
	int totalHeight = 0;
	int lineHeight = 0;

	for (char c : text) {
		if (c == '\n') {
			totalHeight += lineHeight;
			lineHeight = 0;
			continue;
		}

		if (c == '\r') continue;
		if (this->glyphs.find(c) != this->glyphs.end()) {
			lineHeight = std::max(lineHeight, glyphs[c].height);
		}
	}

	return totalHeight + lineHeight;
}

std::shared_ptr<Texture> TextRenderer::getTexture() {
	return this->texture;
}

void TextRenderer::drawText(DrawContext* context, const std::string& text, float x, float y, int r, int g, int b, int a) {
	int lineOffset = 0;
	std::stringstream ss(text);
	std::string line;

	while (std::getline(ss, line, '\n')) {
		this->drawTextInternal(context, line, x, y + lineOffset, r, g, b, a);
		lineOffset += this->fontHeight;
	}
}

void TextRenderer::drawText(DrawContext* context, const std::string& text, float x, float y, int r, int g, int b, int a, bool hasShadow) {
	if (hasShadow) {
		this->drawText(context, text, x + 1, y + 1, 51, 51, 51, 255);
	}
	this->drawText(context, text, x, y, r, g, b, a);
}

void TextRenderer::drawText(DrawContext* context, const std::string& text, float x, float y, int argb) {
	this->drawText(context, text, x, y, Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb));
}

void TextRenderer::drawText(DrawContext* context, const std::string& text, float x, float y, int argb, bool hasShadow) {
	this->drawText(context, text, x, y, Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb), hasShadow);
}

void TextRenderer::drawTextInternal(DrawContext* context, std::string& text, float x, float y, int r, int g, int b, int a) {
	if (text.empty()) return;

	int textHeight = this->getHeight(text);
	float drawX = x;
	float drawY = y;

	if (textHeight > this->fontHeight) {
		drawY -= textHeight - this->fontHeight;
	}

	float baselineY = drawY + this->maxAscent;

	std::vector<float> vertices;
	vertices.reserve(text.size() * 24);

	float texWidth = static_cast<float>(this->texture->getWidth());
	float texHeight = static_cast<float>(this->texture->getHeight());

	for (char c : text) {
		if (c == '\r' || c == '\n') continue;

		auto it = this->glyphs.find(static_cast<unsigned char>(c));
		if (it == this->glyphs.end()) continue;

		const Glyph& glyph = it->second;

		float x1 = drawX + glyph.textureLeft;
		float y1 = baselineY - glyph.textureTop;
		float x2 = x1 + glyph.width;
		float y2 = y1 + glyph.height;

		float s1 = static_cast<float>(glyph.textureX) / texWidth;
		float t1 = static_cast<float>(glyph.textureY) / texHeight;
		float s2 = static_cast<float>(glyph.textureX + glyph.width) / texWidth;
		float t2 = static_cast<float>(glyph.textureY + glyph.height) / texHeight;

		if (glyph.width > 0 && glyph.height > 0) {
			vertices.insert(vertices.end(), {
				x1, y1, s1, t2,
				x1, y2, s1, t1,
				x2, y2, s2, t1,
				
				x2, y2, s2, t1,
				x2, y1, s2, t2,
				x1, y1, s1, t2,
			});
		}

		drawX += glyph.advanceX;
	}

	if (!vertices.empty()) context->drawTextureBatch(*this->texture, vertices, r, g, b, a);
}

}
