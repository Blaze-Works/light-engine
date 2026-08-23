#include <object/Scene.hpp>

#include <algorithm>

namespace blaze::lightEngine {

Scene::Scene() {
	this->matrixAutoUpdate = true;
}

Object3D* Scene::adopt(std::unique_ptr<Object3D> object) {
	if (!object) return nullptr;
	Object3D* raw = object.get();
	this->ownedChildren.push_back(std::move(object));
	this->add(raw);
	return raw;
}

Object3D* Scene::own(std::unique_ptr<Object3D> object) {
	if (!object) return nullptr;
	Object3D* raw = object.get();
	this->ownedChildren.push_back(std::move(object));
	return raw;
}

void Scene::destroy(Object3D* object) {
	if (!object) return;

	this->remove(object);
	this->ownedChildren.erase(std::remove_if(this->ownedChildren.begin(), this->ownedChildren.end(), [object](const std::unique_ptr<Object3D>& p) { return p.get() == object; }), this->ownedChildren.end());
}

void Scene::clearOwned() {
	this->clear();
	this->ownedChildren.clear();
}

} // namespace blaze::lightEngine
