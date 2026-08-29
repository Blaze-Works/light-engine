#include <LightEngine.hpp>
#include <input/Keyboard.hpp>
#include <object/camera/PerspectiveCamera.hpp>
#include <object/controls/OrbitControls.hpp>
#include <object/loaders/GLTFLoader.hpp>
#include <render/Renderer3D.hpp>
#include <util/PathUtil.hpp>
#include <util/math/MathUtils.hpp>

#include <iostream>
#include <string>

namespace bl = blaze::lightEngine;

static bl::LightEngine* g_engine;
bl::Renderer3D renderer;
bl::Scene scene;
bl::PerspectiveCamera camera;
bl::OrbitControls* controls;
bl::GLTFLoader loader;

void init(bl::Window& window) {
	float aspect = window.getAspectRatio();
	renderer.init();

	camera.setAspect(aspect);
	camera.position = {2, 3, 5};

	scene.background = 0xFF1A1A2E;

	bl::GLTFModel model = loader.load(bl::PathUtil::resolveResource("models/damaged_helmet.glb"));
	model.adoptInto(scene);

	controls = new bl::OrbitControls(camera, window);
	controls->target = model.root->position;
	controls->enableDamping = true;
	controls->autoRotate = true;
	controls->autoRotateSpeed = 5.0f;
	controls->update();
	camera.lookAt(0, 0, 0);

	window.onKeyDown([](int key) {
		if (key == bl::Keyboard::Esc) g_engine->scheduleStop();
	});

	window.onResize([&window](int w, int h) {
		float aspect = window.getAspectRatio();
		camera.setAspect(aspect);
	});
}

void render(bl::DrawContext& ctx, int, int, float) {
	renderer.render(scene, camera);
	controls->update();

	std::string text = "Camera Position: X: " + std::to_string(camera.position.x)   + " Y: " + std::to_string(camera.position.y)   + " Z: " + std::to_string(camera.position.z) + 
			 "\nCamera Rotation: X: " + std::to_string(camera.getEuler().x) + " Y: " + std::to_string(camera.getEuler().y) + " Z: " + std::to_string(camera.getEuler().z);
	ctx.drawDebugText(0, bl::LightEngine::getInstance().getWindow().getHeight() - ctx.getDebugTextHeight(text) - 20, text);
}

int main () {
	bl::RunArgs args;
	args.useDebug = true;
	bl::LightEngine engine(args);
	g_engine = &engine;

	engine.getDrawContext()->loadDebugFont(bl::PathUtil::resolveResource("fonts/monospace.ttf"));
	engine.initialize();

	init(engine.getWindow());
	engine.addRenderCallback(render);

	engine.run();
	if (!engine.isRunning()) {
		std::cout << "engine shutdown okay" << std::endl;
		return 0;
	}

	return 1;
}
