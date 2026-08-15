#include <gui/navigation/GuiNavigationType.hpp>

namespace blaze::lightEngine {

bool isMouse(GuiNavigationType type) {
    return type == GuiNavigationType::MOUSE;
}

bool isKeyboard(GuiNavigationType type) {
    return type == GuiNavigationType::KEYBOARD_ARROW || type == GuiNavigationType::KEYBOARD_TAB;
}

}