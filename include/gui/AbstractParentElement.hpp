#pragma once

#include <gui/ParentElement.hpp>

namespace blaze::lightEngine {

class AbstractParentElement : public ParentElement {
public:
    AbstractParentElement();
    bool isDragging() const override;
    void setDragging(bool dragging) override;
    Element* getFocused() const override;
    void setFocused(Element* focused) override;

private:
    Element* focused = nullptr;
    bool dragging = false;
};

} // namespace blaze::lightEngine
