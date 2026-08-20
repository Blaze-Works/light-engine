#pragma once

#include <object/camera/Camera.hpp>
#include <object/Mesh.hpp>
#include <object/Scene.hpp>
#include <gl/ShaderProgram.hpp>

#include <glm/glm.hpp>
#include <vector>

namespace blaze::lightEngine {

class Renderer3D {
public:
	struct Settings {
		bool autoClear = true;
		bool autoClearColor = true;
		bool autoClearDepth = true;
		bool sortObjects = true;
		glm::vec3 lightDirection{0.35f, -1.0f, 0.25f};
		glm::vec4 clearColor{0.12f, 0.12f, 0.14f, 1.0f};
	};

	Settings settings;

	Renderer3D() = default;
	~Renderer3D() = default;

	void init();
	bool isInitialized() const { return shader != nullptr; }
	void setShader(std::shared_ptr<ShaderProgram> shader);
	void render(Scene& scene, Camera& camera);
	void clear(Scene* scene = nullptr);

	std::shared_ptr<ShaderProgram> getShader() const { return shader; }

private:
	struct RenderItem {
		Mesh* mesh = nullptr;
		float z = 0.0f;
	};

	std::shared_ptr<ShaderProgram> shader;
	std::vector<RenderItem> renderList;

	void projectObject(Object3D* object, Camera& camera, std::vector<RenderItem>& list);
	void renderObject(Mesh& mesh, Camera& camera);
};

} // namespace blaze::lightEngine
