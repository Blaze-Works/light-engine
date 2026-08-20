#pragma once

#include <gui/Drawable.hpp>
#include <gui/Element.hpp>
#include <gui/Selectable.hpp>
#include <gui/widget/Widget.hpp>
#include <render/DrawContext.hpp>
#include <text/TextRenderer.hpp>

namespace blaze::lightEngine {

class ClickableWidget : public Drawable, public Element, public Widget, public Selectable {
public:
	ClickableWidget(int x, int y, int width, int height, const std::string& message);
	virtual ~ClickableWidget() = default;

	int getHeight() const override;
	void render(DrawContext& context, int mouseX, int mouseY, float delta) override;

	virtual void renderWidget(DrawContext& context, int mouseX, int mouseY) = 0;

	static void drawScrollableText(DrawContext& context, TextRenderer& textRenderer, const std::string& text, int startX, int startY, int endX, int endY, int color);
	static void drawScrollableText(DrawContext& context, TextRenderer& textRenderer, const std::string& text, int centerX, int startX, int startY, int endX, int endY, int color);
	void drawScrollableText(DrawContext& context, TextRenderer& textRenderer, int xMargin, int color);

	virtual void onClick(double mouseX, double mouseY, int button);
	virtual void onRelease(double mouseX, double mouseY, int button);
	virtual void onDrag(double mouseX, double mouseY, int button, double deltaX, double deltaY);

	bool mouseClicked(double mouseX, double mouseY, int button) override;
	bool mouseReleased(double mouseX, double mouseY, int button) override;
	bool mouseDragged(double mouseX, double mouseY, int button, double deltaX, double deltaY) override;

	int getWidth() const override;
	void setWidth(int width);
	void setHeight(int height);
	void setAlpha(float alpha);
	void setMessage(const std::string& message);
	const std::string& getMessage() const;

	bool isFocused() const override;
	bool isPressed() const;
	bool isHovered() const;
	bool isSelected() const;
	bool isNarratable() const override;
	bool isMouseOver(double mouseX, double mouseY) override;

	void setFocused(bool focused) override;
	Selectable::SelectionType getType() const override;

	int getX() const override;
	void setX(int x) override;
	int getY() const override;
	void setY(int y) override;
	int getRight() const;
	int getBottom() const;

	void forEachChild(const std::function<void(ClickableWidget*)>& consumer) override;

	void setDimensions(int width, int height);
	void setPosition(int x, int y) override;
	void setDimensionsAndPosition(int width, int height, int x, int y);

	int getNavigationOrder() const override;
	void setNavigationOrder(int navigationOrder);
	std::shared_ptr<GuiNavigationPath> getFocusedPath() const override;
	std::shared_ptr<GuiNavigationPath> getNavigationPath(const GuiNavigation& navigation) override;

protected:
	int width;
	int height;
	int x;
	int y;
	std::string message;
	bool hovered;
	bool active = true;
	bool visible = true;
	float alpha = 1.0f;
	int navigationOrder = 0;
	bool focused = false;
	bool pressed = false;
};

} // namespace blaze::lightEngine
