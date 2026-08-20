#pragma once

#include <gui/ScreenPos.hpp>
#include <gui/navigation/NavigationAxis.hpp>
#include <gui/navigation/NavigationDirection.hpp>

#include <optional>

namespace blaze::lightEngine {

class ScreenRect {
public:
	ScreenRect(int sameAxis, int otherAxis, int width, int height);
	ScreenRect(const ScreenPos& position, int width, int height);
	static ScreenRect empty();
	static ScreenRect of(NavigationAxis axis, int sameAxisCoord, int otherAxisCoord, int sameAxisLength, int otherAxisLength);
	ScreenRect add(const NavigationDirection& direction) const;
	int getLength(NavigationAxis axis) const;
	int getBoundingCoordinate(const NavigationDirection& direction) const;
	ScreenRect getBorder(const NavigationDirection& direction) const;
	bool overlaps(const ScreenRect& other) const;
	bool overlaps(const ScreenRect& other, NavigationAxis axis) const;
	int getCenter(NavigationAxis axis) const;
	std::optional<ScreenRect> intersection(const ScreenRect& other) const;
	int getTop() const;
	int getBottom() const;
	int getLeft() const;
	int getRight() const;
	bool contains(int x, int y) const;
	const ScreenPos& getPosition() const;
	int getWidth() const;
	int getHeight() const;

private:
	ScreenPos position;
	int width;
	int height;
	static const ScreenRect* emptyRect;
};

} // namespace blaze::lightEngine
