#pragma once

#include <object/BufferGeometry.hpp>
#include <object/Material.hpp>
#include <object/Skin.hpp>

#include <gl/glad.h>
#include <memory>
#include <vector>

namespace blaze::lightEngine {

class Mesh : public Object3D {
public:
	std::shared_ptr<BufferGeometry> geometry;
	std::shared_ptr<Material> material;
	std::shared_ptr<Skin> skin;

	std::vector<float> morphTargetInfluences;

	Mesh() = default;
	Mesh(std::shared_ptr<BufferGeometry> geometry, std::shared_ptr<Material> material);
	~Mesh() override;

	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;

	void upload();
	void updateMorphTargets();
	void draw() const;
	void dispose();

	bool isUploaded() const { return this->uploaded; }
	bool isSkinned() const { return this->skin != nullptr && this->hasJoints && this->hasWeights; }
	bool hasJointAttrs() const { return this->hasJoints; }
	bool hasWeightAttrss() const { return this->hasWeights; }
	bool hasMorphTargets() const;

	void setMorphInfluences(const std::vector<float>& influences);
	void setMorphInfluence(size_t index, float value);

	BoundingSphere getWorldBoundingSphere() const;
	BoundingBox getWorldBoundingBox() const;

protected:
	BoundingBox computeLocalBoundingBox() const override;

private:
	GLuint vao = 0;
	GLuint vboPosition = 0;
	GLuint vboNormal = 0;
	GLuint vboUv = 0;
	GLuint vboJoints = 0;
	GLuint vboWeights = 0;
	GLuint ebo = 0;
	GLsizei indexCount = 0;
	GLsizei vertexCount = 0;
	bool hasIndex = false;
	bool hasNormal = false;
	bool hasUv = false;
	bool hasJoints = false;
	bool hasWeights = false;
	bool uploaded = false;
	bool morphNeedsUpdate = true;

	std::vector<float> basePosition;
	std::vector<float> baseNormal;
	bool morphBaseCached = false;

	void createBuffers();
	void destroyBuffers();
	void cacheMorphBase();
	void uploadPositionNormal(const std::vector<float>& pos, const std::vector<float>* normal);
};

} // namespace blaze::lightEngine
