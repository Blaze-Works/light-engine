#include <gui/navigation/NavigationAxis.hpp>

namespace blaze::lightEngine {

NavigationAxis getOther(NavigationAxis axis) {
	return axis == NavigationAxis::HORIZONTAL ? NavigationAxis::VERTICAL : NavigationAxis::HORIZONTAL;
}

NavigationDirection getPositiveDirection(NavigationAxis axis) {
	return axis == NavigationAxis::HORIZONTAL ? NavigationDirection::RIGHT : NavigationDirection::DOWN;
}

NavigationDirection getNegativeDirection(NavigationAxis axis) {
	return axis == NavigationAxis::HORIZONTAL ? NavigationDirection::LEFT : NavigationDirection::UP;
}

NavigationDirection getDirection(NavigationAxis axis, bool positive) {
	return positive ? getPositiveDirection(axis) : getNegativeDirection(axis);
}

} // namespace blaze::lightEngine
