#pragma once

#include <GLFW/glfw3.h>

namespace blaze::lightEngine {

class Mouse {
public:
	static bool mouseButtons[GLFW_MOUSE_BUTTON_LAST];
	static bool mouseButtonsPrev[GLFW_MOUSE_BUTTON_LAST];
	static bool mouseButtonsPending[GLFW_MOUSE_BUTTON_LAST];

	static void update(GLFWwindow* window);
	static bool isMousePressed(int button);
	static bool wasMouseJustPressed(int button);
	static bool wasMouseReleased(int button);
	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

	static const int Left =   GLFW_MOUSE_BUTTON_LEFT;
	static const int Middle = GLFW_MOUSE_BUTTON_MIDDLE;
	static const int Right =  GLFW_MOUSE_BUTTON_RIGHT;
};

} // namespace blaze::lightEngine