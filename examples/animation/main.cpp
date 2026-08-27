#include <LightEngine.hpp>
#include <input/Keyboard.hpp>
#include <object/camera/PerspectiveCamera.hpp>
#include <object/controls/OrbitControls.hpp>
#include <object/loaders/GLTFLoader.hpp>
#include <object/Animation.hpp>
#include <render/Renderer3D.hpp>
#include <util/PathUtil.hpp>
#include <util/math/MathUtils.hpp>

#include <iostream>
#include <stdio.h>

namespace bl = blaze::lightEngine;

static size_t l, n;
static int speed = 20;

static bl::LightEngine* g_engine;
bl::Renderer3D renderer;
bl::Scene scene;
bl::PerspectiveCamera camera;
bl::OrbitControls* controls;
bl::GLTFLoader loader;
bl::GLTFModel model;
bl::AnimationPlayer player;
bl::AnimationClip* clip;

std::shared_ptr<bl::BufferGeometry> geo = std::make_shared<bl::BufferGeometry>(bl::BufferGeometry::plane(40, 40));
std::shared_ptr<bl::Material> mat = std::make_shared<bl::Material>(bl::Color::DARK_GRAY);

void init(bl::Window& window) {
	float aspect = window.getAspectRatio();
	renderer.init();

	camera.setAspect(aspect);
	camera.position = {-2, 3, -5};

	scene.background = 0xFF1A1A2E;

	model = loader.load(bl::PathUtil::resolveResource("models/Solider.glb"));
	model.adoptInto(scene);

	if (!model.animations.empty()) {
		clip = &model.animations[0];
		l = model.animations.size();

		player.looping = false;
		player.setClip(clip);
		player.play();
	}

	auto plane = scene.create<bl::Mesh>(geo, mat);
	plane->rotateX(-PI / 2);

	controls = new bl::OrbitControls(camera, window);
	controls->target = {0, 1, 0};
	controls->enableDamping = true;
	controls->update();

	window.onKeyDown([](int key) {
		if (key == bl::Keyboard::Esc) g_engine->scheduleStop();
		if (key == bl::Keyboard::Space) player.playing = !player.playing;
		if (key == bl::Keyboard::Up && speed < 40) speed++;
		if (key == bl::Keyboard::Down && speed > 1) speed--;
	});

	window.onResize([&window](int w, int h) {
		float aspect = window.getAspectRatio();
		camera.setAspect(aspect);
	});
}

void update (float) {
	controls->update();

	if (l > 0) {
		if (player.time < player.getDuration()) return;
		n = ++n % l;
		clip = &model.animations[n];
		player.setClip(clip);
		player.play();
	}
}

void render(bl::DrawContext& ctx, double, double, float dt) {
	player.update(dt * speed / 20);
	renderer.render(scene, camera);

	std::string text = "Animation Clip: " + player.clipName() + "\n" +
			"Time: " + std::to_string(player.time) + " Duration: " + std::to_string(player.getDuration()) + "\n" +
			"Speed: " + std::to_string(speed);

	ctx.drawDebugText(20, bl::LightEngine::getInstance().getWindow().getHeight() - ctx.getDebugTextHeight(text) - 20, text);
}

int main () {
	bl::LightEngine engine;
	engine.initialize();
	engine.getDrawContext()->loadDebugFont(bl::PathUtil::resolveResource("fonts/monospace.ttf"));

	init(engine.getWindow());

	engine.addUpdateCallback(update);
	engine.addRenderCallback(render);

	engine.run();
	if (!engine.isRunning()) {
		std::cout << "engine shutdown okay" << std::endl;
		return 0;
	}
	return 1;
}

