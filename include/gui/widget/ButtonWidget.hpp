#pragma once

#include <gui/widget/PressableWidget.hpp>
#include <functional>

namespace blaze::lightEngine {

class ButtonWidget : public PressableWidget {
public:
	ButtonWidget(int x, int y, int width, int height, const std::string& message, std::function<void(int button)> onPressCallback);
	virtual ~ButtonWidget() = default;

	void onPress(int button) override;
	void renderWidget(DrawContext& context, int mouseX, int mouseY) override;
};

} // namespace blaze::lightEngine
