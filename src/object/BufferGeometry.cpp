#include <object/BufferGeometry.hpp>
#include <util/math/MathUtils.hpp>

#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <limits>
#include <cmath>

namespace blaze::lightEngine {

int BufferGeometry::nextId = 0;

BufferGeometry::BufferGeometry() : uuid(MathUtils::generateUUID()) {}

BufferGeometry& BufferGeometry::setIndex(std::vector<unsigned int> indices) {
	index = std::move(indices);
	return *this;
}

BufferAttribute* BufferGeometry::getAttribute(const std::string& name) {
	auto it = attributes.find(name);
	return it == attributes.end() ? nullptr : &it->second;
}

const BufferAttribute* BufferGeometry::getAttribute(const std::string& name) const {
	auto it = attributes.find(name);
	return it == attributes.end() ? nullptr : &it->second;
}

BufferGeometry& BufferGeometry::setAttribute(const std::string& name, BufferAttribute attribute) {
	attribute.name = name;
	attributes[name] = std::move(attribute);
	return *this;
}

BufferGeometry& BufferGeometry::deleteAttribute(const std::string& name) {
	attributes.erase(name);
	return *this;
}

bool BufferGeometry::hasAttribute(const std::string& name) const {
	return attributes.find(name) != attributes.end();
}

void BufferGeometry::addGroup(int start, int count, int materialIndex) {
	groups.push_back({start, count, materialIndex});
}

void BufferGeometry::clearGroups() {
	groups.clear();
}

void BufferGeometry::setDrawRange(int start, int count) {
	drawRange.start = start;
	drawRange.count = count;
}

BufferGeometry& BufferGeometry::applyMatrix4(const glm::mat4& matrix) {
	if (auto* pos = getAttribute("position")) {
		for (int i = 0; i < pos->count(); i++) {
			glm::vec3 v = pos->getXyz(i);
			glm::vec4 t = matrix * glm::vec4(v, 1.0f);
			pos->setXyz(i, t.x, t.y, t.z);
		}
		pos->needsUpdate = true;
	}

	if (auto* normal = getAttribute("normal")) {
		glm::mat3 normalMat = glm::mat3(glm::transpose(glm::inverse(matrix)));
		for (int i = 0; i < normal->count(); i++) {
			glm::vec3 n = glm::normalize(normalMat * normal->getXyz(i));
			normal->setXyz(i, n.x, n.y, n.z);
		}
		normal->needsUpdate = true;
	}

	if (boundingBoxMin) computeBoundingBox();
	if (boundingSphereCenter) computeBoundingSphere();
	return *this;
}

BufferGeometry& BufferGeometry::rotateX(float angle) {
	return applyMatrix4(glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1, 0, 0)));
}

BufferGeometry& BufferGeometry::rotateY(float angle) {
	return applyMatrix4(glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0)));
}

BufferGeometry& BufferGeometry::rotateZ(float angle) {
	return applyMatrix4(glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 0, 1)));
}

BufferGeometry& BufferGeometry::translate(float x, float y, float z) {
	return applyMatrix4(glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z)));
}

BufferGeometry& BufferGeometry::scale(float x, float y, float z) {
	return applyMatrix4(glm::scale(glm::mat4(1.0f), glm::vec3(x, y, z)));
}

void BufferGeometry::computeBoundingBox() {
	auto* pos = getAttribute("position");
	if (!pos || pos->count() == 0) {
		boundingBoxMin = glm::vec3(0.0f);
		boundingBoxMax = glm::vec3(0.0f);
		return;
	}

	glm::vec3 mn(std::numeric_limits<float>::max());
	glm::vec3 mx(std::numeric_limits<float>::lowest());
	for (int i = 0; i < pos->count(); i++) {
		glm::vec3 v = pos->getXyz(i);
		mn = glm::min(mn, v);
		mx = glm::max(mx, v);
	}
	boundingBoxMin = mn;
	boundingBoxMax = mx;
}

void BufferGeometry::computeBoundingSphere() {
	if (!boundingBoxMin) computeBoundingBox();
	glm::vec3 center = (*boundingBoxMin + *boundingBoxMax) * 0.5f;
	float radius = 0.0f;
	if (auto* pos = getAttribute("position")) {
		for (int i = 0; i < pos->count(); i++) {
			radius = std::max(radius, glm::length(pos->getXyz(i) - center));
		}
	}
	boundingSphereCenter = center;
	boundingSphereRadius = radius;
}

void BufferGeometry::computeVertexNormals() {
	auto* pos = getAttribute("position");
	if (!pos) return;

	std::vector<float> normals(static_cast<size_t>(pos->count()) * 3, 0.0f);

	auto addNormal = [&](unsigned int i0, unsigned int i1, unsigned int i2) {
		glm::vec3 p0 = pos->getXyz(static_cast<int>(i0));
		glm::vec3 p1 = pos->getXyz(static_cast<int>(i1));
		glm::vec3 p2 = pos->getXyz(static_cast<int>(i2));
		glm::vec3 n = glm::cross(p1 - p0, p2 - p0);
		for (unsigned int idx : {i0, i1, i2}) {
			normals[idx * 3 + 0] += n.x;
			normals[idx * 3 + 1] += n.y;
			normals[idx * 3 + 2] += n.z;
		}
	};

	if (hasIndex()) {
		for (size_t i = 0; i + 2 < index.size(); i += 3) {
			addNormal(index[i], index[i + 1], index[i + 2]);
		}
	} else {
		int vc = pos->count();
		for (int i = 0; i + 2 < vc; i += 3) {
			addNormal(i, i + 1, i + 2);
		}
	}

	for (int i = 0; i < pos->count(); i++) {
		glm::vec3 n(normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]);
		n = glm::normalize(n);
		normals[i * 3] = n.x;
		normals[i * 3 + 1] = n.y;
		normals[i * 3 + 2] = n.z;
	}

	setAttribute("normal", BufferAttribute(std::move(normals), 3));
}

int BufferGeometry::vertexCount() const {
	auto it = attributes.find("position");
	if (it == attributes.end()) return 0;
	return it->second.count();
}

void BufferGeometry::dispose() {
	attributes.clear();
	index.clear();
	groups.clear();
	boundingBoxMin.reset();
	boundingBoxMax.reset();
	boundingSphereCenter.reset();
	boundingSphereRadius.reset();
}

// --- Primitives ---

BufferGeometry BufferGeometry::box(float width, float height, float depth) {
	float hx = width * 0.5f, hy = height * 0.5f, hz = depth * 0.5f;

	// 24 vertices for CCW winding order & UV mapping
	float positions[] = {
		// +Z (Front)
		-hx,-hy, hz,   hx,-hy, hz,   hx, hy, hz,  -hx, hy, hz,
		// -Z (Back)
		-hx,-hy,-hz,  -hx, hy,-hz,   hx, hy,-hz,   hx,-hy,-hz,
		// +Y (Top)
		-hx, hy, hz,   hx, hy, hz,   hx, hy,-hz,  -hx, hy,-hz,
		// -Y (Bottom)
		-hx,-hy,-hz,   hx,-hy,-hz,   hx,-hy, hz,  -hx,-hy, hz,
		// +X (Right)
		hx,-hy, hz,   hx,-hy,-hz,   hx, hy,-hz,   hx, hy, hz,
		// -X (Left)
		-hx,-hy,-hz,  -hx,-hy, hz,  -hx, hy, hz,  -hx, hy,-hz,
	};

	float normals[] = {
		0, 0, 1,  0, 0, 1,  0, 0, 1,  0, 0, 1, // +Z
		0, 0,-1,  0, 0,-1,  0, 0,-1,  0, 0,-1, // -Z
		0, 1, 0,  0, 1, 0,  0, 1, 0,  0, 1, 0, // +Y
		0,-1, 0,  0,-1, 0,  0,-1, 0,  0,-1, 0, // -Y
		1, 0, 0,  1, 0, 0,  1, 0, 0,  1, 0, 0, // +X
		-1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, // -X
	};

	float uvs[] = {
		// +Z: BL, BR, TR, TL
		0,0,  1,0,  1,1,  0,1,
		// -Z: BR, TR, TL, BL
		1,0,  1,1,  0,1,  0,0,
		// +Y: BL, BR, TR, TL
		0,0,  1,0,  1,1,  0,1,
		// -Y: TL, TR, BR, BL
		0,1,  1,1,  1,0,  0,0,
		// +X: BL, BR, TR, TL
		0,0,  1,0,  1,1,  0,1,
		// -X: BL, BR, TR, TL
		0,0,  1,0,  1,1,  0,1,
	};

	unsigned int indices[] = {
		0, 1, 2,  0, 2, 3, // +Z
		4, 5, 6,  4, 6, 7, // -Z
		8, 9,10,  8,10,11, // +Y
		12,13,14, 12,14,15, // -Y
		16,17,18, 16,18,19, // +X
		20,21,22, 20,22,23, // -X
	};


	BufferGeometry g;
	g.setAttribute("position", BufferAttribute(positions, 72, 3));
	g.setAttribute("normal", BufferAttribute(normals, 72, 3));
	g.setAttribute("uv", BufferAttribute(uvs, 48, 2));
	g.setIndex(std::vector<unsigned int>(indices, indices + 36));
	g.computeBoundingBox();
	g.computeBoundingSphere();
	return g;
}

BufferGeometry BufferGeometry::plane(float width, float height) {
	float hx = width * 0.5f, hy = height * 0.5f;
	float positions[] = {
		-hx, -hy, 0,  hx, -hy, 0,  hx, hy, 0, -hx, hy, 0,
	};
	float normals[] = {
		0, 0, 1,  0, 0, 1,  0, 0, 1,  0, 0, 1,
	};
	float uvs[] = {
		0, 0, 1, 0, 1, 1, 0, 1,
	};
	unsigned int indices[] = {0, 1, 2, 0, 2, 3};

	BufferGeometry g;
	g.setAttribute("position", BufferAttribute(positions, 12, 3));
	g.setAttribute("normal", BufferAttribute(normals, 12, 3));
	g.setAttribute("uv", BufferAttribute(uvs, 8, 2));
	g.setIndex(std::vector<unsigned int>(indices, indices + 6));
	g.computeBoundingBox();
	g.computeBoundingSphere();
	return g;
}

BufferGeometry BufferGeometry::sphere(float radius, int widthSegments, int heightSegments) {
	widthSegments = std::max(3, widthSegments);
	heightSegments = std::max(2, heightSegments);

	std::vector<float> positions;
	std::vector<float> normals;
	std::vector<float> uvs;
	std::vector<unsigned int> indices;

	positions.reserve((widthSegments + 1) * (heightSegments + 1) * 3);

	for (int y = 0; y <= heightSegments; y++) {
		float v = static_cast<float>(y) / heightSegments;
		float phi = v * PI;
		for (int x = 0; x <= widthSegments; x++) {
			float u = static_cast<float>(x) / widthSegments;
			float theta = u * PI * 2.0f;

			float px = -radius * std::cos(theta) * std::sin(phi);
			float py =  radius * std::cos(phi);
			float pz =  radius * std::sin(theta) * std::sin(phi);

			positions.push_back(px);
			positions.push_back(py);
			positions.push_back(pz);

			glm::vec3 n = glm::normalize(glm::vec3(px, py, pz));
			normals.push_back(n.x);
			normals.push_back(n.y);
			normals.push_back(n.z);

			uvs.push_back(u);
			uvs.push_back(1.0f - v);
		}
	}

	for (int y = 0; y < heightSegments; y++) {
		for (int x = 0; x < widthSegments; x++) {
			unsigned int a = y * (widthSegments + 1) + x;
			unsigned int b = a + widthSegments + 1;
			unsigned int c = a + 1;
			unsigned int d = b + 1;

			if (y != 0) {
				indices.push_back(a);
				indices.push_back(b);
				indices.push_back(c);
			}
			if (y != heightSegments - 1) {
				indices.push_back(c);
				indices.push_back(b);
				indices.push_back(d);
			}
		}
	}

	BufferGeometry g;
	g.setAttribute("position", BufferAttribute(std::move(positions), 3));
	g.setAttribute("normal", BufferAttribute(std::move(normals), 3));
	g.setAttribute("uv", BufferAttribute(std::move(uvs), 2));
	g.setIndex(std::move(indices));
	g.computeBoundingBox();
	g.computeBoundingSphere();
	return g;
}

} // namespace blaze::lightEngine
