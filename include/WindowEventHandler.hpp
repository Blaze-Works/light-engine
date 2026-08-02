#pragma once

namespace blaze::lightEngine {

class WindowEventHandler {
public:
    virtual ~WindowEventHandler() = default;

    virtual void onWindowFocusChanged(bool /*focused*/) {}
    virtual void onResolutionChanged() {}
    virtual void onCursorEnterChanged() {}
    virtual void onCursorPosChanged() {}
};

} // namespace blaze::lightEngine
