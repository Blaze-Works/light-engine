#include <LightEngine.hpp>
#include <input/Keyboard.hpp>
#include <memory>
#include <object/camera/PerspectiveCamera.hpp>
#include <render/Renderer3D.hpp>
#include <util/PathUtil.hpp>
#include <util/math/MathUtils.hpp>

#include <iostream>

namespace bl = blaze::lightEngine;
static int i = 0;
static bl::LightEngine* g_engine;
bl::Renderer3D renderer;
bl::Scene scene;
bl::PerspectiveCamera camera;
bl::Mesh* cube;
bl::Mesh* plane;

auto geo = std::make_shared<bl::BufferGeometry>(bl::BufferGeometry::box(1, 1, 1));
auto mat = std::make_shared<bl::Material>(0xFF4CAF50);

void init(bl::Window& window) {
	float aspect = window.getAspectRatio();

	renderer.init();

	camera.setAspect(aspect);
	camera.position = {2, 4, 3};
	camera.lookAt(0, 0, 0);

	scene.background = 0xFF1A1A2E;

	cube = scene.create<bl::Mesh>(geo, mat);

	cube->position = {0, 0, 0};

	window.onKeyUp([](int key) {
		if (key == bl::Keyboard::Esc && g_engine) g_engine->scheduleStop();
	});

	window.onResize([&window](int w, int h) {
		float aspect = window.getAspectRatio();
		camera.setAspect(aspect);
	});
}

void update(float delta) {
	cube->rotateY(delta).rotateX(delta);
}

void render(bl::DrawContext& ctx, int x, int y, float delta) {
	renderer.render(scene, camera);
}

int main() {
	bl::LightEngine engine;
	g_engine = &engine;
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
