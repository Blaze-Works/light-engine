#pragma once

namespace blaze::lightEngine {

enum class GuiNavigationType {
	NONE,
	MOUSE,
	KEYBOARD_ARROW,
	KEYBOARD_TAB
};

bool isMouse(GuiNavigationType type);
bool isKeyboard(GuiNavigationType type);

} // namespace blaze::lightEngine
