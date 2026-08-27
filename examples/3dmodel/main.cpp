#include <LightEngine.hpp>
#include <input/Keyboard.hpp>
#include <object/Scene.hpp>
#include <object/camera/PerspectiveCamera.hpp>
#include <object/loaders/GLTFLoader.hpp>
#include <render/Renderer3D.hpp>
#include <util/PathUtil.hpp>
#include <util/math/MathUtils.hpp>

#include <iostream>
#include <string>

namespace bl = blaze::lightEngine;

float angle = 0;
int r = 5;

static bl::LightEngine* g_engine;
std::vector<std::shared_ptr<bl::Texture>>* tex;
bl::Renderer3D renderer;
bl::Scene scene;
bl::PerspectiveCamera camera(75, 0, 0.1f, 1000);
bl::GLTFLoader loader;

void init(bl::Window& window) {
	float aspect = window.getAspectRatio();

	renderer.init();

	camera.setAspect(aspect);
	camera.position = {0, r, r};
	camera.lookAt(0, 0, 0);

	scene.background = 0xFF1A1A2E;

	bl::GLTFModel model = loader.load(bl::PathUtil::resolveResource("models/utah_teapot.glb"));
	model.adoptInto(scene);

	window.onKeyUp([](int key) {
		if (key == bl::Keyboard::Esc && g_engine) g_engine->scheduleStop();
		if (key == bl::Keyboard::Up && r > 1) r--;
		if (key == bl::Keyboard::Down && r < 100) r++;
	});

	window.onResize([&window](int w, int h) {
		float aspect = window.getAspectRatio();
		camera.setAspect(aspect);
	});
}

void update(float delta) {
	angle += PI * delta / 10;
	float px = std::cos(angle) * r;
	float pz = std::sin(angle) * r;

	camera.position = {px, r, pz};
	camera.lookAt(0, 0, 0);

}

void render(bl::DrawContext& ctx, int x, int y, float delta) {
	renderer.render(scene, camera);

	std::string text = "Camera radius: " + std::to_string(r);
	ctx.drawDebugText(0, bl::LightEngine::getInstance().getWindow().getHeight() - ctx.getDebugTextHeight(text) - 20, text);
}

int main() {
	bl::RunArgs args;
	args.useDebug = true;
	bl::LightEngine engine(args);
	g_engine = &engine;

	engine.getDrawContext()->loadDebugFont(bl::PathUtil::resolveResource("monospace.ttf"));
	engine.initialize();

	init(engine.getWindow());
	engine.addRenderCallback(render);
	engine.addUpdateCallback(update);
	engine.run();

	if (!engine.isRunning()) {
		std::cout << "engine shutdown: okay" << std::endl;
		return 0;
	}

	return 1;
}
