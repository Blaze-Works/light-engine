#include <gui/widget/ButtonWidget.hpp>
#include <input/Mouse.hpp>

namespace blaze::lightEngine {

ButtonWidget::ButtonWidget(int x, int y, int width, int height, const std::string& message, std::function<void(int button)> onPressCallback): PressableWidget(x, y, width, height, message) {
	this->onPressCallback = onPressCallback;
}

void ButtonWidget::onPress(int button) {
	if (this->onPressCallback) this->onPressCallback(button);
}

void ButtonWidget::renderWidget(DrawContext& context, int mouseX, int mouseY) {
	PressableWidget::renderWidget(context, mouseX, mouseY);
	if (isHovered()) {
		context.drawQuad(getX(), getY(), getWidth(), getHeight(), 0x08FFFFFF);
	}
}

} // namespace blaze::lightEngine
