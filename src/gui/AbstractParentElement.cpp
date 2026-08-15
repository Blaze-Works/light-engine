#include <gui/AbstractParentElement.hpp>

namespace blaze::lightEngine {

AbstractParentElement::AbstractParentElement() = default;

bool AbstractParentElement::isDragging() const {
    return this->dragging;
}

void AbstractParentElement::setDragging(bool dragging) {
    this->dragging = dragging;
}

Element* AbstractParentElement::getFocused() const {
    return this->focused;
}

void AbstractParentElement::setFocused(Element* focused) {
    if (this->focused != nullptr) {
        this->focused->setFocused(false);
    }

    if (focused != nullptr) {
        focused->setFocused(true);
    }

    this->focused = focused;
}

} // namespace blaze::lightEngine
