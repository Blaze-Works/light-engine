#include <WindowSettings.hpp>

#include <sstream>

namespace blaze::lightEngine {

WindowSettings::WindowSettings(std::size_t width, std::size_t height, std::size_t fullscreenWidth, std::size_t fullscreenHeight, bool fullscreen)
	: width(width), height(height), fullscreenWidth(fullscreenWidth), fullscreenHeight(fullscreenHeight), fullscreen(fullscreen) {}

std::string WindowSettings::describe() const {
	std::ostringstream os;
	os << width << "x" << height;
	if (fullscreen) {
		os << " [fullscreen=" << fullscreenWidth << "x" << fullscreenHeight << "]";
	}
	return os.str();
}

} // namespace blaze::lightEngine

