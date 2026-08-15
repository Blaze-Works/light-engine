#pragma once

#include <gui/ScreenRect.hpp>
#include <gui/navigation/GuiNavigation.hpp>

#include <memory>

namespace blaze::lightEngine {

class GuiNavigationPath;
class GuiNavigation;

class Element {
public:
    static constexpr long MAX_DOUBLE_CLICK_INTERVAL = 250L;
    virtual ~Element() = default;

    virtual void mouseMoved(double mouseX, double mouseY) {}
    virtual bool mouseClicked(double mouseX, double mouseY, int button) { return false; }
    virtual bool mouseReleased(double mouseX, double mouseY, int button) { return false; }
    virtual bool mouseDragged(double mouseX, double mouseY, int button, double deltaX, double deltaY) { return false; }
    virtual bool mouseScrolled(double mouseX, double mouseY, double horizontalAmount, double verticalAmount) { return false; }
    virtual bool keyPressed(int keyCode, int scanCode, int modifiers) { return false; }
    virtual bool keyReleased(int keyCode, int scanCode, int modifiers) { return false; }
    virtual bool charTyped(char chr, int modifiers) { return false; }
    virtual bool isMouseOver(double mouseX, double mouseY) = 0;

    virtual void setFocused(bool focused) = 0;
    virtual int getNavigationOrder() const = 0;
    virtual bool isFocused() const = 0;
    virtual std::shared_ptr<GuiNavigationPath> getFocusedPath() const;
    virtual std::shared_ptr<GuiNavigationPath> getNavigationPath(const GuiNavigation& navigation) = 0;
    virtual ScreenRect getNavigationFocus() const;
};

} // namespace blaze::lightEngine
