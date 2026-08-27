#pragma once

#include <map>

namespace blaze::lightEngine {

class VertexFormat;

class VertexFormats {
public:
	static VertexFormat POSITION;
	static VertexFormat POSITION_TEXTURE;
	static VertexFormat POSITION_COLOR;
	static VertexFormat POSITION_TEXTURE_COLOR;
	static VertexFormat LINES;

};

} // namespace blaze::lightEngine