#pragma once

#include <object/Animation.hpp>
#include <object/Mesh.hpp>
#include <object/Scene.hpp>
#include <object/Skin.hpp>
#include <texture/Texture.hpp>

#include <memory>
#include <string>
#include <vector>

namespace blaze::lightEngine {

struct GLTFModel {
	Object3D* root;
	std::vector<std::unique_ptr<Object3D>> ownedNodes;
	std::vector<std::shared_ptr<BufferGeometry>> geometries;
	std::vector<std::shared_ptr<Material>> materials;
	std::vector<std::shared_ptr<Texture>> textures;
	std::vector<std::shared_ptr<Skin>> skins;
	std::vector<AnimationClip> animations;
	std::string name;

	void adoptInto(class Scene& scene);
};

class GLTFLoader {
public:
	GLTFLoader() = default;

	GLTFModel load(const std::string& path);
	GLTFModel loadFromMemory(const unsigned char* data, size_t size, const std::string& name = "");
	void setBasePath(std::string path) { this->basePath = std::move(path); }

private:
	std::string basePath;

};

} // namespace blaze::lightEngine
