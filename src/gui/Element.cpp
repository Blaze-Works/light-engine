#include <gui/Element.hpp>

namespace blaze::lightEngine {

std::shared_ptr<GuiNavigationPath> Element::getFocusedPath() const {
    return nullptr;
}

ScreenRect Element::getNavigationFocus() const {
    return ScreenRect::empty();
}

} // namespace blaze::lightEngine
