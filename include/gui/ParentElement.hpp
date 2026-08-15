#pragma once

#include <gui/Element.hpp>
#include <gui/navigation/GuiNavigationPath.hpp>
#include <gui/ScreenRect.hpp>
#include <vector>
#include <optional>

namespace blaze::lightEngine {

class ParentElement : public Element {
public:
    virtual ~ParentElement() = default;

    virtual std::vector<Element*> getChildren() = 0;

    virtual std::optional<Element*> hoveredElement(double mouseX, double mouseY) {
        for (Element* element : this->getChildren()) {
            if (element->isMouseOver(mouseX, mouseY)) return element;
        }
        return std::nullopt;
    }

    bool mouseClicked(double mouseX, double mouseY, int button) override;
    bool mouseReleased(double mouseX, double mouseY, int button) override;
    bool mouseDragged(double mouseX, double mouseY, int button, double deltaX, double deltaY) override;
    bool mouseScrolled(double mouseX, double mouseY, double horizontalAmount, double verticalAmount) override;
    bool keyPressed(int keyCode, int scanCode, int modifiers) override;
    bool keyReleased(int keyCode, int scanCode, int modifiers) override;
    bool charTyped(char chr, int modifiers) override;

    virtual bool isDragging() const = 0;
    virtual void setDragging(bool dragging) = 0;
    virtual Element* getFocused() const = 0;
    virtual void setFocused(Element* focused) = 0;

    void setFocused(bool focused) override {}
    bool isFocused() const override;
    std::shared_ptr<GuiNavigationPath> getFocusedPath() const override;
    std::shared_ptr<GuiNavigationPath> getNavigationPath(const GuiNavigation& navigation) override;

private:
    std::shared_ptr<GuiNavigationPath> computeNavigationPath(const GuiNavigationTab& navigation);
    std::shared_ptr<GuiNavigationPath> computeNavigationPath(const GuiNavigationArrow& navigation);
    std::shared_ptr<GuiNavigationPath> computeChildPath(const ScreenRect& focus, const NavigationDirection& direction, Element* focused, const GuiNavigation& navigation);
    std::shared_ptr<GuiNavigationPath> computeInitialChildPath(const ScreenRect& focus, const NavigationDirection& direction, Element* focused, const GuiNavigation& navigation);
};

} // namespace blaze::lightEngine
