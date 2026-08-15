#pragma once

#include <gui/widget/ClickableWidget.hpp>
#include <functional>

namespace blaze::lightEngine {

class PressableWidget : public ClickableWidget {
public:
    PressableWidget(int x, int y, int width, int height, const std::string& message);
    virtual ~PressableWidget() = default;

    virtual void onPress(int button) = 0;
    void onClick(double mouseX, double mouseY, int button) override;
    void onRelease(double mouseX, double mouseY, int button) override;
    void renderWidget(DrawContext& context, int mouseX, int mouseY) override;
    void drawMessage(DrawContext& context, TextRenderer& textRenderer, int color);

protected:
    std::function<void(int)> onPressCallback = nullptr;
};

} // namespace blaze::lightEngine
