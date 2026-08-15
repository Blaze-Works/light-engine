#include <input/Mouse.hpp>

namespace blaze::lightEngine {

bool Mouse::mouseButtons[GLFW_MOUSE_BUTTON_LAST] = {};
bool Mouse::mouseButtonsPrev[GLFW_MOUSE_BUTTON_LAST] = {};
bool Mouse::mouseButtonsPending[GLFW_MOUSE_BUTTON_LAST] = {};

void Mouse::update(GLFWwindow* window) {
    for (int i = 0; i < GLFW_MOUSE_BUTTON_LAST; ++i) {
        Mouse::mouseButtonsPrev[i] = Mouse::mouseButtons[i];

        Mouse::mouseButtons[i] = Mouse::mouseButtonsPending[i];

        if (!Mouse::mouseButtons[i] && glfwGetMouseButton(window, i) == GLFW_PRESS) {
            Mouse::mouseButtons[i] = true;
            Mouse::mouseButtonsPending[i] = true;
        }
    }
}

bool Mouse::isMousePressed(int button) {
    return Mouse::mouseButtons[button];
}

bool Mouse::wasMouseJustPressed(int button) {
    return Mouse::mouseButtons[button] && !Mouse::mouseButtonsPrev[button];
}

bool Mouse::wasMouseReleased(int button) {
    return !mouseButtons[button] && mouseButtonsPrev[button];
}

void Mouse::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button >= 0 && button < GLFW_MOUSE_BUTTON_LAST) {
        mouseButtons[button] = (action != GLFW_RELEASE);
    }
}

} // namespace blaze::lightEngine