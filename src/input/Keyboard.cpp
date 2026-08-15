#include <GLFW/glfw3.h>
#include <input/Keyboard.hpp>
#include <util/Logger.hpp>

namespace blaze::lightEngine {

bool Keyboard::keys[GLFW_KEY_LAST] = {};
bool Keyboard::keysPrev[GLFW_KEY_LAST] = {};
bool Keyboard::keysPending[GLFW_KEY_LAST] = {};

void Keyboard::update(GLFWwindow* window) {
    for (int i = 32; i <= GLFW_KEY_LAST; ++i) {
        Keyboard::keysPrev[i] = Keyboard::keys[i];

        Keyboard::keys[i] = Keyboard::keysPending[i];

        if (!Keyboard::keys[i] && glfwGetKey(window, i) == GLFW_PRESS) {
            Keyboard::keys[i] = true;
            Keyboard::keysPending[i] = true;
        }
    }
}

bool Keyboard::isPressed(int key) {
    return Keyboard::keys[key];
}

bool Keyboard::isComboPressed(int keys...) {
    return false;
}

bool Keyboard::wasJustPressed(int key) {
    return Keyboard::keys[key] && !Keyboard::keysPrev[key];
}

bool Keyboard::wasJustReleased(int key) {
    return !Keyboard::keys[key] && Keyboard::keysPrev[key];
}

void Keyboard::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key >= 32 && key <= GLFW_KEY_LAST) {
        if (action == GLFW_PRESS) {
            Keyboard::keysPending[key] = true;
        } else if (action == GLFW_RELEASE) {
            Keyboard::keysPending[key] = false;
        }
    }
}


} // namespace blaze::lightEngine
