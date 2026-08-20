#pragma once

#include <gui/navigation/NavigationDirection.hpp>

namespace blaze::lightEngine {

class GuiNavigation {
public:
	virtual ~GuiNavigation() = default;
};

class GuiNavigationArrow : public GuiNavigation {
public:
	explicit GuiNavigationArrow(const NavigationDirection& direction);
	const NavigationDirection& getDirection() const;

private:
	NavigationDirection direction;
};

class GuiNavigationTab : public GuiNavigation {
public:
	explicit GuiNavigationTab(bool forward);
	const NavigationDirection& getDirection() const;
	bool getForward() const;

private:
	bool forward;
};

class GuiNavigationDown : public GuiNavigation {
public:
	GuiNavigationDown();
	const NavigationDirection& getDirection() const;
};

} // namespace blaze::lightEngine
