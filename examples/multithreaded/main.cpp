#include <LightEngine.hpp>
#include <input/Keyboard.hpp>
#include <memory>
#include <object/camera/PerspectiveCamera.hpp>
#include <render/Renderer3D.hpp>
#include <util/PathUtil.hpp>
#include <util/math/MathUtils.hpp>

#include <cmath>
#include <iostream>
#include <random>

namespace bl = blaze::lightEngine;

static bl::LightEngine* g_engine = nullptr;
bl::Renderer3D renderer;
bl::Scene scene;
bl::PerspectiveCamera camera;

auto geo = std::make_shared<bl::BufferGeometry>(bl::BufferGeometry::box(1, 1, 1));

int r = 10;
int count = 100;
int prevCount = 0;
const int maxCount = 500;
float angle = 0;

int randInt() {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> distrib(-20, 20);
	return distrib(gen);
}

std::shared_ptr<bl::Material> randMat() {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> distrib(0xFF000000, 0xFFFFFFFF);

	return std::make_shared<bl::Material>(distrib(gen));
}

void init(bl::Window& window) {
	float aspect = window.getAspectRatio();

	renderer.init();

	camera.setAspect(aspect);
	camera.position = {0, r, r};
	camera.lookAt(0, 0, 0);

	scene.background = 0xFF1A1A2E;

	for (int i = 0;i < maxCount;i++) {
		auto cube = scene.create<bl::Mesh>(geo, randMat());
		cube->position = {randInt(), randInt(), randInt()};
	}

	window.onKeyUp([](int key) {
		if (key == bl::Keyboard::Esc && g_engine) g_engine->scheduleStop();
		if (key == bl::Keyboard::Up && r > 1) r--;
		if (key == bl::Keyboard::Down && r < 100) r++;
		if (key == bl::Keyboard::Left && count > 1) count--;
		if (key == bl::Keyboard::Right && count < maxCount) count++;
	});

	window.onResize([&window](int w, int h) {
		float aspect = window.getAspectRatio();
		camera.setAspect(aspect);
	});
}

void update(float delta) {
	angle += PI / 8 * delta;
	float px = std::cos(angle) * r;
	float pz = std::sin(angle) * r;

	camera.position = {px, r, pz};
	camera.lookAt(0, 0, 0);

	auto it = scene.children.begin();
	int i = 0;

	while (it != scene.children.end() && prevCount != count) {
		auto child = *it++;
		child->visible = i < count;
		i++;
	}
	prevCount = count;
}

void render(bl::DrawContext& ctx, int x, int y, float delta) {
	renderer.render(scene, camera);
	auto s = renderer.getStats();

	std::string text = "Count: " + std::to_string(count) +
		"\nCamera radius: " + std::to_string(r) +
		"\nTotal Candidates: " + std::to_string(s.totalCandidates) +
		"\nCulled Objects: " + std::to_string(s.frustumCulled);
	ctx.drawDebugText(0, bl::LightEngine::getInstance().getWindow().getHeight() - ctx.getDebugTextHeight(text) - 20, text);
}

int main() {
	bl::RunArgs args;
	args.useDebug = true;
	bl::LightEngine engine(args);
	g_engine = &engine;

	engine.getDrawContext()->loadDebugFont(bl::PathUtil::resolveResource("fonts/monospace.ttf"));
	engine.initialize();

	init(engine.getWindow());
	engine.addRenderCallback(render);
	engine.addUpdateCallback(update);
	engine.shouldRenderAsync(true);
	engine.setMaxFPS(120);
	engine.setMaxUPS(50);
	engine.run();

	if (!engine.isRunning()) {
		std::cout << "engine shutdown: okay" << std::endl;
		return 0;
	}

	return 1;
}
