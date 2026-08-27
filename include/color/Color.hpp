#pragma once

#include <color/Argb.hpp>
#include <glm/glm.hpp>

namespace blaze::lightEngine {

class Color {
private:
	glm::vec4 color;

public:
	Color() = default;
	Color(glm::vec4 color): color(color) {}
	Color(int argb) { this->color = {Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb)}; }
	Color(int red, int green, int blue, int alpha) { this->color = {red, green, blue, alpha}; }
	~Color() = default;

	Color& operator=(const Color&) = delete;
	void operator=(int argb) { this->color = {Argb::getRed(argb), Argb::getGreen(argb), Argb::getBlue(argb), Argb::getAlpha(argb)}; };
	void operator=(const glm::vec4& v) { this->color = v; }

	glm::vec4 getColor() { return this->color; }
	int getRed() { return this->color.r; }
	int getGreen() { return this->color.g; }
	int getBlue() { return this->color.b; }
	int getAlpha() { return this->color.a; }
	Color operator+(const Color other) { return Color(this->color + other.color); }

	static Color BLACK;;
	static Color WHITE;;
	static Color RED;;
	static Color LIME;;
	static Color BLUE;;
	static Color YELLOW;;
	static Color CYAN;
	static Color MAGENTA;;
	static Color SILVER;;
	static Color GRAY;	;
	static Color MAROON;;
	static Color OLIVE;	;
	static Color GREEN;;
	static Color PURPLE;;
	static Color TEAL;
	static Color NAVY;

	static Color INDIAN_RED;
	static Color LIGHT_CORAL;
	static Color SALMON;
	static Color DARKS_ALMON;
	static Color LIGHT_SALMON;
	static Color CRIMSON;;
	static Color FIREBRICK;
	static Color DARK_RED;
	static Color PINK;
	static Color LIGHT_PINK;
	static Color HOT_PINK;
	static Color DEEP_PINK;
	static Color MEDIUM_VIOLET_RED;
	static Color PALE_VIOLET_RED;

	static Color CORAL;
	static Color TOMATO;
	static Color ORANGE_RED;
	static Color DARK_ORANGE;
	static Color ORANGE;
	static Color GOLD;
	static Color LIGHT_YELLOW ;
	static Color LEMON_CHIFFON;
	static Color LIGHT_GOLDEN_ROD_YELLOW;
	static Color PAPAYA_WHIP;
	static Color MOCCASIN;
	static Color PEACHPUFF;
	static Color PALE_GOLDEN_ROD;
	static Color KHAKI;;
	static Color DARK_KHAKI;

	static Color LAVENDER;
	static Color THISTLE;;
	static Color PLUM;
	static Color VIOLET;
	static Color ORCHID;
	static Color MEDIUMO_RCHID;
	static Color MEDIUM_PURPLE;
	static Color REBECCA_PURPLE;
	static Color BLUE_VIOLET;
	static Color DARK_VIOLET;
	static Color DARK_ORCHID;
	static Color DARKM_AGENTA;
	static Color INDIGO;
	static Color SLATE_BLUE;
	static Color DARK_SLATE_BLUE;
	static Color MEDIUM_SLATE_BLUE;

	static Color GREEN_YELLOW;
	static Color CHARTREUSE;
	static Color LAWN_GREEN;
	static Color LIME_GREEN;
	static Color PALE_GREEN;
	static Color LIGHT_GREEN;
	static Color MEDIUM_SPRING_GREEN;
	static Color SPRING_GREEN;
	static Color MEDIUM_SEA_GREEN;
	static Color SEA_GREEN;
	static Color FOREST_GREEN;
	static Color DARK_GREEN;
	static Color YELLOW_GREEN;
	static Color OLIVEDRAB;
	static Color DARK_OLIVE_GREEN;
	static Color MEDIUM_AQUA_MARINE;
	static Color DARK_SEA_GREEN;
	static Color LIGHT_SEA_GREEN;
	static Color DARK_CYAN;

	static Color LIGHT_CYAN;
	static Color PALE_TURQUOISE;
	static Color AQUAMARINE;
	static Color TURQUOISE;
	static Color MEDIUM_TURQUOISE;
	static Color DARK_TURQUOISE;
	static Color CADET_BLUE;
	static Color STEEL_BLUE;
	static Color LIGHT_STEEL_BLUE;
	static Color POWDER_BLUE;
	static Color LIGHT_BLUE;
	static Color SKY_BLUE;
	static Color LIGHT_SKY_BLUE;
	static Color DEEP_SKY_BLUE;
	static Color DODGER_BLUE;
	static Color CORN_FLOWER_BLUE;
	static Color ROYAL_BLUE;
	static Color MEDIUM_BLUE;
	static Color DARK_BLUE;
	static Color MIDNIGHT_BLUE;

	static Color CORNSILK;
	static Color BLANCHED_ALMOND;
	static Color BISQUE;
	static Color NAVAJO_WHITE;
	static Color WHEAT;
	static Color BURLY_WOOD;
	static Color TAN;
	static Color ROSY_BROWN;
	static Color SANDY_BROWN;
	static Color GOLDEN_ROD;
	static Color DARK_GOLDEN_ROD;
	static Color PERU;
	static Color CHOCOLATE;
	static Color SADDLE_BROWN;
	static Color SIENNA;
	static Color BROWN;

	static Color GAINSBORO;
	static Color LIGHT_GRAY;
	static Color DARK_GRAY;
	static Color DIM_GRAY;
	static Color LIGHT_SLATE_GRAY;
	static Color SLATE_GRAY;
	static Color DARK_SLATE_GRAY;
};

} // namespace blaze::lightEngine
