#pragma once

#include <util/Window.hpp>

namespace blaze::lightEngine {

class WindowProvider {
public:
    explicit WindowProvider(WindowEventHandler* eventHandler);
    Window createWindow(WindowSettings settings, std::string title);
    Window createWindow(WindowSettings settings);

private:
    WindowEventHandler* eventHandler;
};

} // namespace blaze::lightEngine