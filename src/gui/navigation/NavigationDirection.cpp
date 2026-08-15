#include <gui/navigation/NavigationDirection.hpp>
#include <gui/navigation/NavigationAxis.hpp>

#include <functional>

namespace blaze::lightEngine {

NavigationAxis getAxis(NavigationDirection direction) {
    switch (direction) {
        case NavigationDirection::UP:
        case NavigationDirection::DOWN:
            return NavigationAxis::VERTICAL;
        case NavigationDirection::LEFT:
        case NavigationDirection::RIGHT:
            return NavigationAxis::HORIZONTAL;
    }
    return NavigationAxis::HORIZONTAL;
}

NavigationDirection getOpposite(NavigationDirection direction) {
    switch (direction) {
        case NavigationDirection::UP:    return NavigationDirection::DOWN;
        case NavigationDirection::DOWN:  return NavigationDirection::UP;
        case NavigationDirection::LEFT:  return NavigationDirection::RIGHT;
        case NavigationDirection::RIGHT: return NavigationDirection::LEFT;
    }
    return NavigationDirection::UP;
}

bool isPositive(NavigationDirection direction) {
    switch (direction) {
        case NavigationDirection::UP:    return false;
        case NavigationDirection::DOWN:  return true;
        case NavigationDirection::LEFT:  return false;
        case NavigationDirection::RIGHT: return true;
    }
    return false;
}

bool isAfter(NavigationDirection direction, int a, int b) {
    if (isPositive(direction)) return a > b; else return b > a;
}

bool isBefore(NavigationDirection direction, int a, int b) {
    if (isPositive(direction)) return a < b; else return b < a;
}

int getComponent(NavigationDirection direction, int x, int y) {
    NavigationAxis axis = getAxis(direction);
    return axis == NavigationAxis::HORIZONTAL ? x : y;
}

const std::function<int(int,int)>& getComparator(NavigationDirection direction) {
    static std::function<int(int,int)> compUp = [](int a, int b){ return a == b ? 0 : (a < b ? -1 : 1); };
    static std::function<int(int,int)> compDown = [](int a, int b){ return a == b ? 0 : (a > b ? -1 : 1); };
    // For simplicity return compUp for most; callers should adapt
    return compUp;
}

} // namespace blaze::lightEngine
