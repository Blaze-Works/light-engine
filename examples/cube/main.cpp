#include <LightEngine.hpp>
#include <input/Keyboard.hpp>
#include <object/camera/PerspectiveCamera.hpp>
#include <object/BufferGeometry.hpp>
#include <object/Scene.hpp>
#include <render/Renderer3D.hpp>

#include <iostream>

namespace bl = blaze::lightEngine;

static bl::LightEngine* g_engine = nullptr;
bl::Renderer3D renderer;
bl::Scene scene;
bl::PerspectiveCamera camera(75, 0, 0.1f, 1000);

void init() {
    float aspect = g_engine->getWindow().getAspectRatio();

    renderer.init();

    camera.setAspect(aspect);
    camera.position = {0, 0, 3};
    camera.lookAt(0, 0, 0);

    scene.background = 0xFF1A1A2E;

    auto geo = std::make_shared<bl::BufferGeometry>(bl::BufferGeometry::box(1, 1, 1));
    auto mat = std::make_shared<bl::Material>(0xFF4CAF50);
    auto* cube = scene.create<bl::Mesh>(geo, mat);
    cube->position = {0, 0, 0};

    g_engine->getWindow().onKeyUp([](int key) {
        if (key == bl::Keyboard::Esc && g_engine) g_engine->scheduleStop();
    });
}

void render(bl::DrawContext& ctx, int x, int y, float delta) {
    bl::Mesh* cube = static_cast<bl::Mesh*>(scene.children.back());
    cube->rotateY(0.06f).rotateX(0.06f);
    renderer.render(scene, camera);
}

int main() {
    bl::LightEngine engine;
    g_engine = &engine;
    engine.initialize();

    init();
    engine.addRenderCallback(render);
    engine.run();

    if (!engine.isRunning()) {
        std::cout << "engine shutdown: okay" << std::endl;
        return 0;
    }

    return 1;
}