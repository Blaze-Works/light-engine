#include <gui/widget/PressableWidget.hpp>

namespace blaze::lightEngine {

PressableWidget::PressableWidget(int x, int y, int width, int height, const std::string& message): ClickableWidget(x, y, width, height, message) {}

void PressableWidget::onClick(double /*mouseX*/, double /*mouseY*/, int /*button*/) {}

void PressableWidget::renderWidget(DrawContext& context, int /*mouseX*/, int /*mouseY*/) {
    int i = this->active ? 0xFFD8D8D8 : 0xFFFFFFFF;
    this->drawMessage(context, *context.textRenderer, i);
    context.drawQuad(this->getX(), this->getY(), this->getWidth(), this->getHeight(), 0XFF080808);
}

void PressableWidget::onRelease(double mouseX, double mouseY, int button) {
    if (!this->active || !this->visible) return;
    bool releasedInside = mouseX >= this->getX() && mouseY >= this->getY() && mouseX < this->getX() + this->getWidth() && mouseY < this->getY() + this->getHeight();
    if (releasedInside) {
        this->onPress(button);
    }
}

void PressableWidget::drawMessage(DrawContext& context, TextRenderer& textRenderer, int color) {
    this->drawScrollableText(context, textRenderer, 2, color);
}

} // namespace blaze::lightEngine
