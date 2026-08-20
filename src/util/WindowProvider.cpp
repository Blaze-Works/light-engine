#include <util/WindowProvider.hpp>

namespace blaze::lightEngine {

WindowProvider::WindowProvider(WindowEventHandler* eventHandler) {
	this->eventHandler = eventHandler;
}

Window WindowProvider::createWindow(WindowSettings settings, std::string title) {
	Window window(this->eventHandler, settings, title);
	return window;
}

Window WindowProvider::createWindow(WindowSettings settings) {
	return this->createWindow(settings, "Untitled");
}

} // namespace blaze::lightEngine