#include <LightEngine.hpp>
#include <gui/widget/ButtonWidget.hpp>
#include <input/Keyboard.hpp>
#include <texture/TextureUtil.hpp>
#include <util/PathUtil.hpp>
#include <iostream>
#include <memory>

namespace bl = blaze::lightEngine;

class PauseScreen : public bl::Screen {
public:
    PauseScreen() : Screen("Pause") {}

    void init() override {
        this->addDrawableChild<bl::ButtonWidget>(350, 200, 160, 40, "Resume", [this](int) {
            this->close();
        });
    }

    void render(bl::DrawContext& ctx, int x, int y, float d) override {
        Screen::render(ctx, x, y, d);
        ctx.drawQuad(0, 0, this->engine->getWindow().getWidth(), this->engine->getWindow().getHeight(), 0x80000000);
        ctx.drawText(0, 0, "qwertyuiop", 0xFFFFFFFF);
    }
};

class TitleScreen : public bl::Screen {
public:
    TitleScreen() : Screen("Title") {}

    void init() override {
        this->addDrawableChild<bl::ButtonWidget>(150, 150, 160, 40, "Open Pause", [this](int) {
            this->engine->pushScreen(std::make_unique<PauseScreen>());
        });

        this->addDrawableChild<bl::ButtonWidget>(150, 200, 160, 40, "Quit", [this](int) {
            this->engine->scheduleStop();
        });
    }
};

int main() {
    bl::LightEngine engine;
    engine.initialize();
    bl::PathUtil::setResourceDir("./");

    engine.getDrawContext()->loadFont(bl::PathUtil::resolveResource("segoe.ttf"));
    engine.setScreen(std::make_unique<TitleScreen>());

    engine.getWindow().onKeyUp([](int key) {
        if (key == bl::Keyboard::Esc) bl::LightEngine::getInstance().scheduleStop();
    });

    engine.run();

    if (!engine.isRunning()) {
        std::cout << "engine shutdown ok" << std::endl;
        return 0;
    }
    return 1;
}

