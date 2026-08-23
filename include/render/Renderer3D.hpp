#pragma once

#include <object/camera/Camera.hpp>
#include <object/Frustum.hpp>
#include <object/Mesh.hpp>
#include <object/Scene.hpp>
#include <gl/ShaderProgram.hpp>

#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace blaze::lightEngine {

class Renderer3D {
public:
	struct Settings {
		bool autoClear = true;
		bool autoClearColor = true;
		bool autoClearDepth = true;
		bool sortObjects = true;
		bool frustumCulling = true;
		bool hierarchicalCulling = false;
		glm::vec3 lightDirection{0.35f, -1.0f, 0.25f};
		glm::vec4 clearColor{0.12f, 0.12f, 0.14f, 1.0f};
	};

	Settings settings;

	Renderer3D() = default;
	~Renderer3D() = default;

	void init();
	bool isInitialized() const { return this->shader != nullptr; }
	void setShader(std::shared_ptr<ShaderProgram> shader);
	void render(Scene& scene, Camera& camera);
	void clear(Scene* scene = nullptr);
	const Frustum& getFrustum() const { return this->frustum; }

	struct Stats {
		int drawn = 0;
		int frustumCulled = 0;
		int hierarchicalCulled = 0;
		int totalCandidates = 0;
	};
	const Stats& getStats() const { return this->stats; }

private:
	struct RenderItem {
		Mesh* mesh = nullptr;
		float z = 0.0f;
	};

	std::shared_ptr<ShaderProgram> shader;
	std::vector<RenderItem> renderList;
	Frustum frustum;
	Stats stats;

	bool cullObject(Object3D* object, Camera& camera, std::vector<RenderItem>& item);
	void projectObject(Object3D* object, Camera& camera, std::vector<RenderItem>& list);
	void renderObject(Mesh& mesh, Camera& camera);
};

} // namespace blaze::lightEngine
