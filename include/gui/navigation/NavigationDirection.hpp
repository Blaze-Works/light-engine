#pragma once

#include <functional>

namespace blaze::lightEngine {

enum class NavigationAxis;

enum class NavigationDirection {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

NavigationDirection getOpposite(NavigationDirection direction);
bool isPositive(NavigationDirection direction);
bool isAfter(NavigationDirection direction, int a, int b);
bool isBefore(NavigationDirection direction, int a, int b);
int getComponent(NavigationDirection direction, int x, int y);
const std::function<int(int, int)>& getComparator(NavigationDirection direction);
NavigationAxis getAxis(NavigationDirection direction);

} // namespace blaze::lightEngine
