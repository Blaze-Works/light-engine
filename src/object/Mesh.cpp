#include <object/Mesh.hpp>

#include <algorithm>
#include <limits>

namespace blaze::lightEngine {

Mesh::Mesh(std::shared_ptr<BufferGeometry> geometry, std::shared_ptr<Material> material): geometry(std::move(geometry)), material(std::move(material)) {
	if (this->geometry) this->upload();
}

Mesh::~Mesh() {
	this->destroyBuffers();
}

void Mesh::createBuffers() {
	if (this->vao == 0) glGenVertexArrays(1, &this->vao);
	if (this->vboPosition == 0) glGenBuffers(1, &this->vboPosition);
	if (this->vboNormal == 0) glGenBuffers(1, &this->vboNormal);
	if (this->vboUv == 0) glGenBuffers(1, &this->vboUv);
	if (this->vboJoints == 0) glGenBuffers(1, &this->vboJoints);
	if (this->vboWeights == 0) glGenBuffers(1, &this->vboWeights);
	if (this->ebo == 0) glGenBuffers(1, &this->ebo);
}

void Mesh::destroyBuffers() {
	if (this->vao) { glDeleteVertexArrays(1, &this->vao); vao = 0; }
	if (this->vboPosition) { glDeleteBuffers(1, &this->vboPosition); vboPosition = 0; }
	if (this->vboNormal) { glDeleteBuffers(1, &this->vboNormal); vboNormal = 0; }
	if (this->vboUv) { glDeleteBuffers(1, &this->vboUv); vboUv = 0; }
	if (this->vboJoints) { glDeleteBuffers(1, &this->vboJoints); vboJoints = 0; }
	if (this->vboWeights) { glDeleteBuffers(1, &this->vboWeights); this->vboWeights = 0; }
	if (this->ebo) { glDeleteBuffers(1, &this->ebo); this->ebo = 0; }
	this->uploaded = false;
}

void Mesh::upload() {
	if (!this->geometry) return;

	this->createBuffers();
	glBindVertexArray(this->vao);

	auto uploadAttr = [](GLuint vbo, const BufferAttribute* attr, GLuint location) -> bool {
		if (!attr || attr->count() <= 0) {
			glDisableVertexAttribArray(location);
			return false;
		}

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(attr->byteLength()), attr->data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(location);
		glVertexAttribPointer(location, attr->itemSize, GL_FLOAT, attr->normalized ? GL_TRUE : GL_FALSE, 0, nullptr);
		
		return true;
	};

	const BufferAttribute* pos = this->geometry->getAttribute("position");
	if (pos && pos->count() > 0) {
		uploadAttr(this->vboPosition, pos, 0);
		this->vertexCount = static_cast<GLsizei>(pos->count());
	} else {
		this->vertexCount = 0;
	}

	const BufferAttribute* normal = this->geometry->getAttribute("normal");
	this->hasNormal = uploadAttr(this->vboNormal, normal, 1);

	const BufferAttribute* uv = this->geometry->getAttribute("uv");
	this->hasUv = uploadAttr(this->vboUv, uv, 2);

	const BufferAttribute* joints = this->geometry->getAttribute("joints");
	this->hasJoints = uploadAttr(this->vboJoints, joints, 3);

	const BufferAttribute* weights = this->geometry->getAttribute("weights");
	this->hasWeights = uploadAttr(this->vboWeights, weights, 4);

	this->hasIndex = this->geometry->hasIndex();
	if (this->hasIndex) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(this->geometry->index.size() * sizeof(unsigned int)), this->geometry->index.data(), GL_STATIC_DRAW);
		this->indexCount = static_cast<GLsizei>(this->geometry->index.size());
	} else {
		this->indexCount = 0;
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	uploaded = true;
}

void Mesh::draw() const {
	if (!this->uploaded || this->vao == 0) return;

	glBindVertexArray(this->vao);

	GLsizei count = this->hasIndex ? this->indexCount : this->vertexCount;
	int start = this->geometry ? this->geometry->drawRange.start : 0;

	if (this->geometry && this->geometry->drawRange.count >= 0) {
		count = this->geometry->drawRange.count;
	}

	if (this->hasIndex) {
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, reinterpret_cast<const void*>(static_cast<uintptr_t>(start) * sizeof(unsigned int)));
	} else {
		glDrawArrays(GL_TRIANGLES, start, count);
	}

	glBindVertexArray(0);
}

void Mesh::dispose() {
	this->destroyBuffers();
	if (this->geometry) this->geometry->dispose();
}

BoundingSphere Mesh::getWorldBoundingSphere() const {
	BoundingSphere out;
	if (!this->geometry) return out;

	if (!this->geometry->boundingSphereCenter || !this->geometry->boundingSphereRadius) const_cast<BufferGeometry*>(geometry.get())->computeBoundingSphere();
	if (!this->geometry->boundingSphereCenter || !this->geometry->boundingSphereRadius) return out;

	glm::vec3 localCenter = *geometry->boundingSphereCenter;
	float localRadius = *geometry->boundingSphereRadius;
	glm::vec3 worldCenter = glm::vec3(this->matrixWorld * glm::vec4(localCenter, 1.0f));
	glm::vec3 scale(
		glm::length(glm::vec3(this->matrixWorld[0])),
		glm::length(glm::vec3(this->matrixWorld[1])),
		glm::length(glm::vec3(this->matrixWorld[2]))
	);

	float maxScale = std::max(scale.x, std::max(scale.y, scale.z));
	out.center = worldCenter;
	out.radius = localRadius * maxScale;
	out.valid = true;
	return out;
}

BoundingBox Mesh::getWorldBoundingBox() const {
	BoundingBox out;
	if (!this->geometry) return out;

	if (!this->geometry->boundingBoxMin || !this->geometry->boundingBoxMax) return out;

	glm::vec3 mn = *this->geometry->boundingBoxMin;
	glm::vec3 mx = *this->geometry->boundingBoxMax;

	glm::vec3 corners[8] = {
		{mn.x, mn.y, mn.z}, {mx.x, mn.y, mn.z},
		{mn.x, mx.y, mn.z}, {mx.x, mx.y, mn.z},
		{mn.x, mn.y, mx.z}, {mx.x, mn.y, mx.z},
		{mn.x, mx.y, mx.z}, {mx.x, mx.y, mx.z},
	};

	glm::vec3 wmin(std::numeric_limits<float>::max());
	glm::vec3 wmax(std::numeric_limits<float>::lowest());
	
	for (const glm::vec3& c : corners) {
		glm::vec3 w = glm::vec3(this->matrixWorld * glm::vec4(c, 1.0f));
		wmin = glm::min(wmin, w);
		wmax = glm::max(wmax, w);
	}

	out.min = wmin;
	out.max = wmax;
	out.valid = true;
	return out;
}

BoundingBox Mesh::computeLocalBoundingBox() const {
	BoundingBox out;
	if (!this->geometry) return out;
	if (!this->geometry->boundingBoxMin || !this->geometry->boundingBoxMax) const_cast<BufferGeometry*>(this->geometry.get())->computeBoundingBox();
	if (!this->geometry->boundingBoxMin || !this->geometry->boundingBoxMax) return out;

	out.min = *this->geometry->boundingBoxMin;
	out.max = *this->geometry->boundingBoxMax;
	out.valid = true;
	return out;
}

} // namespace blaze::lightEngine
