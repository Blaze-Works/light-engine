#pragma once

namespace blaze::lightEngine {

class DrawContext;

class Drawable {
public:
	virtual ~Drawable() = default;
	virtual void render(DrawContext& context, int mouseX, int mouseY, float delta) = 0;
};

} // namespace blaze::lightEngine
