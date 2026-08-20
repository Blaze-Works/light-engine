#pragma once

#include <gui/navigation/NavigationDirection.hpp>

namespace blaze::lightEngine {

enum class NavigationAxis {
	HORIZONTAL,
	VERTICAL
};

NavigationAxis getOther(NavigationAxis axis);
NavigationDirection getPositiveDirection(NavigationAxis axis);
NavigationDirection getNegativeDirection(NavigationAxis axis);
NavigationDirection getDirection(NavigationAxis axis, bool positive);

} // namespace blaze::lightEngine
