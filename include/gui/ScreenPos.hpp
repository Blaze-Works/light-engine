#pragma once

#include <gui/navigation/NavigationAxis.hpp>
#include <gui/navigation/NavigationDirection.hpp>

namespace blaze::lightEngine {

class ScreenPos {
public:
	ScreenPos(int x, int y);
	static ScreenPos of(NavigationAxis axis, int sameAxis, int otherAxis);
	ScreenPos add(const NavigationDirection& direction) const;
	int getComponent(NavigationAxis axis) const;
	int getX() const;
	int getY() const;

private:
	int x;
	int y;
};

} // namespace blaze::lightEngine
