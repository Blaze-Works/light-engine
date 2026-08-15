#pragma once

#include <gui/ScreenRect.hpp>
#include <functional>

namespace blaze::lightEngine {

class ClickableWidget;

class Widget {
public:
    virtual ~Widget() = default;
    virtual void setX(int x) = 0;
    virtual void setY(int y) = 0;
    virtual int getX() const = 0;
    virtual int getY() const = 0;
    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
    virtual ScreenRect getNavigationFocus() const { return ScreenRect(getX(), getY(), getWidth(), getHeight()); }
    virtual void setPosition(int x, int y) { setX(x); setY(y); }
    virtual void forEachChild(const std::function<void(ClickableWidget*)>& consumer) = 0;
};

} // namespace blaze::lightEngine
