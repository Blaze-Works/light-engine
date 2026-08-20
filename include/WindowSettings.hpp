#pragma once

#include <cstddef>
#include <string>

namespace blaze::lightEngine {

struct WindowSettings {
	std::size_t width;
	std::size_t height;
	std::size_t fullscreenWidth;
	std::size_t fullscreenHeight;
	bool fullscreen;

	WindowSettings(
		std::size_t width = 720,
		std::size_t height = 520,
		std::size_t fullscreenWidth = 0,
		std::size_t fullscreenHeight = 0,
		bool fullscreen = false
	);

	[[nodiscard]] std::string describe() const;
};

} // namespace blaze::lightEngine

