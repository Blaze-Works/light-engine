#include <gui/ScreenPos.hpp>

#include <stdexcept>

namespace blaze::lightEngine {

ScreenPos::ScreenPos(int x, int y): x(x), y(y) {}

ScreenPos ScreenPos::of(NavigationAxis axis, int sameAxis, int otherAxis) {
	ScreenPos* screenPos;
	switch (axis) {
		case NavigationAxis::HORIZONTAL: screenPos = new ScreenPos(sameAxis, otherAxis); break;
		case NavigationAxis::VERTICAL: screenPos = new ScreenPos(otherAxis, sameAxis); break;
		default: throw std::runtime_error("Invalid axis");
	}

	return *screenPos;
}

ScreenPos ScreenPos::add(const NavigationDirection& direction) const {
	ScreenPos* screenPos;
	switch (direction) {
		case NavigationDirection::DOWN: screenPos  = new ScreenPos(this->x, this->y + 1); break;
		case NavigationDirection::UP: screenPos    = new ScreenPos(this->x, this->y - 1); break;
		case NavigationDirection::LEFT: screenPos  = new ScreenPos(this->x - 1, this->y); break;
		case NavigationDirection::RIGHT: screenPos = new ScreenPos(this->x + 1, this->y); break;
		default: throw std::runtime_error("Invalid direction");
	}

	return *screenPos;
}

int ScreenPos::getComponent(NavigationAxis axis) const {
	int pos;
	switch (axis) {
		case NavigationAxis::HORIZONTAL: pos = this->x; break;
		case NavigationAxis::VERTICAL: pos = this->y; break;
		default: throw std::runtime_error("Invalid axis");
	}

	return pos;
}

int ScreenPos::getX() const {
	return this->x;
}

int ScreenPos::getY() const {
	return this->y;
}

}
