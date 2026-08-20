#include <gui/navigation/GuiNavigation.hpp>

namespace blaze::lightEngine {

GuiNavigationArrow::GuiNavigationArrow(const NavigationDirection& direction): direction(direction) {}

const NavigationDirection& GuiNavigationArrow::getDirection() const {
	return direction;
}

GuiNavigationTab::GuiNavigationTab(bool forward): forward (forward) {}

const NavigationDirection& GuiNavigationTab::getDirection() const {
	static NavigationDirection dir = NavigationDirection::DOWN;
	return dir;
}

bool GuiNavigationTab::getForward() const {
	return forward;
}

GuiNavigationDown::GuiNavigationDown() {}

const NavigationDirection& GuiNavigationDown::getDirection() const {
	static NavigationDirection dir = NavigationDirection::DOWN;
	return dir;
}

} // namespace blaze::lightEngine
