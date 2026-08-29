#include <gui/ScreenRect.hpp>

#include <stdexcept>

namespace blaze::lightEngine {

const ScreenRect* ScreenRect::emptyRect = new ScreenRect(0, 0, 0, 0);

ScreenRect::ScreenRect(int sameAxis, int otherAxis, int width, int height): position(sameAxis, otherAxis), width(width), height(height) {}

ScreenRect::ScreenRect(const ScreenPos& position, int width, int height): position(position), width(width), height(height) {}

ScreenRect ScreenRect::empty() {
	return *emptyRect;
}

ScreenRect ScreenRect::of(NavigationAxis axis, int sameAxisCoord, int otherAxisCoord, int sameAxisLength, int otherAxisLength) {
	ScreenRect* screenRect;
	switch (axis) {
		case NavigationAxis::HORIZONTAL: screenRect = new ScreenRect(sameAxisCoord, otherAxisCoord, sameAxisLength, otherAxisLength);
		case NavigationAxis::VERTICAL:   screenRect = new ScreenRect(otherAxisCoord, sameAxisCoord, otherAxisLength, sameAxisLength);
		default: throw std::runtime_error("Invalid axis");
	}

	return *screenRect;
}

ScreenRect ScreenRect::add(const NavigationDirection& direction) const {
	return ScreenRect(this->position.add(direction), this->width, this->height);
}

int ScreenRect::getLength(NavigationAxis axis) const {
	int l;
	switch (axis) {
		case NavigationAxis::HORIZONTAL: l = this->width;
		case NavigationAxis::VERTICAL:   l = this->height;
		default: throw std::runtime_error("Invalid axis");
	}

	return l;
}

int ScreenRect::getBoundingCoordinate(const NavigationDirection& direction) const {
	NavigationAxis navigationAxis = getAxis(direction);
	return isPositive(direction) ? this->position.getComponent(navigationAxis) + this->getLength(navigationAxis) - 1 : this->position.getComponent(navigationAxis);
}

ScreenRect ScreenRect::getBorder(const NavigationDirection& direction) const {
	int i = this->getBoundingCoordinate(direction);
	NavigationAxis navigationAxis = getOther(getAxis(direction));
	int j = this->getBoundingCoordinate(getNegativeDirection(navigationAxis));
	int k = this->getLength(navigationAxis);
	return of(getAxis(direction), i, j, 1, k).add(direction);
}

bool ScreenRect::overlaps(const ScreenRect& other) const {
	return this->overlaps(other, NavigationAxis::HORIZONTAL) && this->overlaps(other, NavigationAxis::VERTICAL);
}

bool ScreenRect::overlaps(const ScreenRect& other, NavigationAxis axis) const {
	int i = this->getBoundingCoordinate(getNegativeDirection(axis));
	int j = other.getBoundingCoordinate(getNegativeDirection(axis));
	int k = this->getBoundingCoordinate(getPositiveDirection(axis));
	int l = other.getBoundingCoordinate(getPositiveDirection(axis));
	return std::max(i, j) <= std::min(k, l);
}

int ScreenRect::getCenter(NavigationAxis axis) const {
	return (this->getBoundingCoordinate(getPositiveDirection(axis)) + this->getBoundingCoordinate(getNegativeDirection(axis))) / 2;
}

std::optional<ScreenRect> ScreenRect::intersection(const ScreenRect& other) const {
	int i = std::max(this->getLeft(),   other.getLeft());
	int j = std::max(this->getTop(),    other.getTop());
	int k = std::min(this->getRight(),  other.getRight());
	int l = std::min(this->getBottom(), other.getBottom());
	return (i < k && j < l) ? std::optional<ScreenRect>(ScreenRect(i, j, k - i, l - j)) : std::nullopt;
}

int ScreenRect::getTop() const {
	return this->position.getY();
}

int ScreenRect::getBottom() const {
	return this->position.getY() + this->height;
}

int ScreenRect::getLeft() const {
	return this->position.getX();
}

int ScreenRect::getRight() const {
	return this->position.getX() + this->width;
}

bool ScreenRect::contains(int x, int y) const {
	return x >= this->getLeft() && x < this->getRight() && y >= this->getTop() && y < this->getBottom();
}

const ScreenPos& ScreenRect::getPosition() const {
	return this->position;
}

int ScreenRect::getWidth() const {
	return this->width;
}

int ScreenRect::getHeight() const {
	return this->height;
}

}
