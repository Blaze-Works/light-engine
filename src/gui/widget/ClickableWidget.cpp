#include <LightEngine.hpp>
#include <gui/widget/ClickableWidget.hpp>
#include <util/math/MathUtils.hpp>
#include <algorithm>

namespace blaze::lightEngine {

ClickableWidget::ClickableWidget(int x, int y, int width, int height, const std::string& message): width(width), height(height), x (x), y (y), message(message), hovered(false), pressed(false) {}

int ClickableWidget::getHeight() const {
    return height;
}

void ClickableWidget::render(DrawContext& context, int mouseX, int mouseY, float delta) {
    if (!visible) return;
    this->hovered = this->isMouseOver(mouseX, mouseY);
    renderWidget(context, mouseX, mouseY);
}

void ClickableWidget::drawScrollableText(DrawContext& context, TextRenderer& textRenderer, const std::string& text, int startX, int startY, int endX, int endY, int color) {
    drawScrollableText(context, textRenderer, text, (startX + endX) / 2, startX, startY, endX, endY, color);
}

void ClickableWidget::drawScrollableText(DrawContext& context, TextRenderer& textRenderer, const std::string& text, int centerX, int startX, int startY, int endX, int endY, int color) {
    int i = textRenderer.getWidth(text);
    int q = startY + endY;
    int j = (q - 9) / 2 + 1;
    int k = endX - startX;
    int l;
    if (i > k) {
        l = i - k;
        double d = LightEngine::getInstance().getTime();
        double e = std::max((double)l * 0.5, 3.0);
        double f = std::sin(1.5707963267948966 * std::cos(6.283185307179586 * d / e)) / 2.0 + 0.5;
        double g = MathUtils::clamp((float) f, 0.0f, (float) l);
        context.drawText(startX - (int) g, j, text, color);
    } else {
        l = (int) MathUtils::clamp((float) centerX, startX + (float) i / 2, endX - (float) i / 2);
        context.drawText(l - (float) textRenderer.getWidth(text) / 2, j, text, color);
    }
}

void ClickableWidget::drawScrollableText(DrawContext& context, TextRenderer& textRenderer, int xMargin, int color) {
    int i = this->getX() + xMargin;
    int j = this->getX() + this->getWidth() - xMargin;
    drawScrollableText(context, textRenderer, this->getMessage(), i, this->getY(), j, this->getY() + this->getHeight(), color);
}

void ClickableWidget::onClick(double /*mouseX*/, double /*mouseY*/, int /*button*/) {}
void ClickableWidget::onRelease(double /*mouseX*/, double /*mouseY*/, int /*button*/) {}
void ClickableWidget::onDrag(double /*mouseX*/, double /*mouseY*/, int /*button*/, double /*deltaX*/, double /*deltaY*/) {}

bool ClickableWidget::mouseClicked(double mouseX, double mouseY, int button) {
    if (active && visible) {
        bool clicked = mouseX >= (double)getX() && mouseY >= (double)getY() && mouseX < (double)(getX() + getWidth()) && mouseY < (double)(getY() + getHeight());
        if (clicked) {
            this->pressed = true;
            onClick(mouseX, mouseY, button);
            return true;
        }
    }
    return false;
}

bool ClickableWidget::mouseReleased(double mouseX, double mouseY, int button) {
    if (this->pressed) {
        this->pressed = false;
        onRelease(mouseX, mouseY, button);
        return true;
    }
    return false;
}

bool ClickableWidget::mouseDragged(double mouseX, double mouseY, int button, double deltaX, double deltaY) {
    if (this->pressed) {
        onDrag(mouseX, mouseY, button, deltaX, deltaY);
        return true;
    }
    return false;
}

bool ClickableWidget::isPressed() const {
    return this->pressed;
}

int ClickableWidget::getWidth() const { return width; }
void ClickableWidget::setWidth(int w) { width = w; }
void ClickableWidget::setHeight(int h) { height = h; }
void ClickableWidget::setAlpha(float a) { alpha = a; }
void ClickableWidget::setMessage(const std::string& m) { message = m; }
const std::string& ClickableWidget::getMessage() const { return message; }

bool ClickableWidget::isFocused() const { return focused; }
bool ClickableWidget::isHovered() const { return hovered; }
bool ClickableWidget::isSelected() const { return isHovered() || isFocused(); }
bool ClickableWidget::isNarratable() const { return visible && active; }
bool ClickableWidget::isMouseOver(double mouseX, double mouseY) { return mouseX >= this->getX() && mouseY >= this->getY() && mouseX < this->getX() + width && mouseY < this->getY() + height; }

void ClickableWidget::setFocused(bool f) { focused = f; }
Selectable::SelectionType ClickableWidget::getType() const { return isFocused() ? Selectable::SelectionType::FOCUSED : (isHovered() ? Selectable::SelectionType::HOVERED : Selectable::SelectionType::NONE); }

int ClickableWidget::getX() const { return x; }
void ClickableWidget::setX(int x) { x = x; }
int ClickableWidget::getY() const { return y; }
void ClickableWidget::setY(int y) { y = y; }
int ClickableWidget::getRight() const { return getX() + getWidth(); }
int ClickableWidget::getBottom() const { return getY() + getHeight(); }

void ClickableWidget::forEachChild(const std::function<void(ClickableWidget*)>& consumer) { consumer(this); }

void ClickableWidget::setDimensions(int w, int h) { setWidth(w); setHeight(h); }
void ClickableWidget::setPosition(int x, int y) { setX(x); setY(y); }
void ClickableWidget::setDimensionsAndPosition(int w, int h, int x, int y) { setDimensions(w,h); setPosition(x,y); }

int ClickableWidget::getNavigationOrder() const { return navigationOrder; }
void ClickableWidget::setNavigationOrder(int n) { navigationOrder = n; }

std::shared_ptr<GuiNavigationPath> ClickableWidget::getFocusedPath() const { return Element::getFocusedPath(); }
std::shared_ptr<GuiNavigationPath> ClickableWidget::getNavigationPath(const GuiNavigation& navigation) {
    return nullptr;
}

} // namespace blaze::lightEngine
