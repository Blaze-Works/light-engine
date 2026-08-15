#include <object/Scene.hpp>

#include <algorithm>

namespace blaze::lightEngine {

Scene::Scene() {
    matrixAutoUpdate = true;
}

Object3D* Scene::adopt(std::unique_ptr<Object3D> object) {
    if (!object) return nullptr;
    Object3D* raw = object.get();
    ownedChildren.push_back(std::move(object));
    this->add(raw);
    return raw;
}

void Scene::destroy(Object3D* object) {
    if (!object) return;

    this->remove(object);

    ownedChildren.erase(std::remove_if(ownedChildren.begin(), ownedChildren.end(), [object](const std::unique_ptr<Object3D>& p) { return p.get() == object; }), ownedChildren.end());
}

void Scene::clearOwned() {
    this->clear();
    ownedChildren.clear();
}

} // namespace blaze::lightEngine
