#include <LightEngine.hpp>
#include <input/Keyboard.hpp>
#include <texture/TextureUtil.hpp>
#include <iostream>

namespace bl = blaze::lightEngine;

std::shared_ptr<bl::Texture> tex;

void init() {
	bl::Texture t = bl::TextureUtil::loadTexture("../examples/texture/test.png");
	tex = std::make_shared<bl::Texture>(t);
}

void render(bl::DrawContext& ctx, int, int, float) {
	ctx.drawTexture(*tex, 0, 0, 0xFFFFFFFF);
}

int main() {
	bl::LightEngine engine;
	engine.initialize();

	init();
	engine.addRenderCallback(render);

	engine.getWindow().onKeyUp([](int key) {
		if (key == bl::Keyboard::Esc) bl::LightEngine::getInstance().scheduleStop();
	});

	engine.run();
	if (engine.isRunning()) {
		std::cout << "engine shutdown okay" << std::endl;
		return 0;
	}

	return 1;
}
