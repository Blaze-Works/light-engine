#include <object/loaders/GLTFLoader.hpp>
#include <object/Animation.hpp>
#include <object/Scene.hpp>
#include <texture/TextureUtil.hpp>

#include <nlohmann/json.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <algorithm>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <vector>

namespace blaze::lightEngine {

using json = nlohmann::json;

namespace {

constexpr uint32_t GLTF_MAGIC = 0x46546C67;
constexpr uint32_t CHUNK_JSON = 0x4E4F534A;
constexpr uint32_t CHUNK_BIN  = 0x004E4942;

constexpr int COMP_BYTE = 5120;
constexpr int COMP_UBYTE = 5121;
constexpr int COMP_SHORT = 5122;
constexpr int COMP_USHORT = 5123;
constexpr int COMP_UINT = 5125;
constexpr int COMP_FLOAT = 5126;

int typeComponentCount(const std::string& type) {
	if (type == "SCALAR") return 1;
	if (type == "VEC2") return 2;
	if (type == "VEC3") return 3;
	if (type == "VEC4") return 4;
	if (type == "MAT4") return 16;
	return 0;
}

int componentByteSize(int t) {
	switch (t) {
		case COMP_BYTE:  case COMP_UBYTE:  return 1;
		case COMP_SHORT: case COMP_USHORT: return 2;
		case COMP_UINT:  case COMP_FLOAT:  return 4;
		default: return 0;
	}
}

float readFloat(const unsigned char* p, int type, bool normalized) {
	switch (type) {
		case COMP_FLOAT: { float v; std::memcpy(&v, p, 4); return v; }
		case COMP_UBYTE: return normalized ? (*p) / 255.0f : float(*p);
		case COMP_BYTE: {
			int8_t v = static_cast<int8_t>(*p);
			return normalized ? std::max(v / 127.0f, -1.0f) : float(v);
		}
		case COMP_USHORT: {
			uint16_t v; std::memcpy(&v, p, 2);
			return normalized ? v / 65535.0f : float(v);
		}
		case COMP_SHORT: {
			int16_t v; std::memcpy(&v, p, 2);
			return normalized ? std::max(v / 32767.0f, -1.0f) : float(v);
		}
		case COMP_UINT: {
			uint32_t v; std::memcpy(&v, p, 4); return float(v);
		}
		default: return 0.f;
	}
}

uint32_t readU32(const unsigned char* p, int type) {
	switch (type) {
		case COMP_UBYTE: return *p;
		case COMP_USHORT: { uint16_t v; std::memcpy(&v, p, 2); return v; }
		case COMP_UINT: { uint32_t v; std::memcpy(&v, p, 4); return v; }
		default: return uint32_t(readFloat(p, type, false));
	}
}

struct ParsedGLB {
	json root;
	std::vector<unsigned char> bin;
};

ParsedGLB parseGLB(const unsigned char* data, size_t size) {
	if (size < 12) throw std::runtime_error("GLB: file too small");
	uint32_t magic, version, length;
	std::memcpy(&magic, data, 4);
	std::memcpy(&version, data + 4, 4);
	std::memcpy(&length, data + 8, 4);
	if (magic != GLTF_MAGIC) throw std::runtime_error("GLB: bad magic");
	if (version != 2) throw std::runtime_error("GLB: only version 2 supported");

	ParsedGLB out;
	size_t offset = 12;
	while (offset + 8 <= size && offset + 8 <= length) {
		uint32_t chunkLen, chunkType;
		std::memcpy(&chunkLen, data + offset, 4);
		std::memcpy(&chunkType, data + offset + 4, 4);
		offset += 8;

		if (offset + chunkLen > size) throw std::runtime_error("GLB: chunk overflow");
		if (chunkType == CHUNK_JSON) {
			std::string s(reinterpret_cast<const char*>(data + offset), chunkLen);
			while (!s.empty() && (s.back() == ' ' || s.back() == '\0')) s.pop_back();
			out.root = json::parse(s);
		} else if (chunkType == CHUNK_BIN) {
			out.bin.assign(data + offset, data + offset + chunkLen);
		}

		offset += chunkLen;
	}
	if (out.root.is_null()) throw std::runtime_error("GLB: missing JSON chunk");
	return out;
}

void accessorMeta(const json& root, const std::vector<unsigned char>& bin, int accessorIndex, const unsigned char*& base, size_t& stride, int& count, int& compType, int& components, bool& normalized) {
	const json& acc = root.at("accessors").at(accessorIndex);
	count =         acc.at("count").get<int>();
	compType =      acc.at("componentType").get<int>();
	components =    typeComponentCount(acc.at("type").get<std::string>());
	normalized =    acc.value("normalized", false);
	int accOffset = acc.value("byteOffset", 0);

	if (!acc.contains("bufferView")) throw std::runtime_error("GLTF: sparse/zero-bufferView accessors not supported");

	const json& bv = root.at("bufferViews").at(acc.at("bufferView").get<int>());
	int bvOffset = bv.value("byteOffset", 0);
	int bvStride = bv.value("byteStride", 0);
	int elemSize = componentByteSize(compType) * components;
	stride = bvStride > 0 ? size_t(bvStride) : size_t(elemSize);
	size_t start = size_t(bvOffset + accOffset);

	if (start >= bin.size()) throw std::runtime_error("GLTF: accessor past end of buffer");
	base = bin.data() + start;
}

std::vector<float> readFloatAccessor(const json& root, const std::vector<unsigned char>& bin, int idx) {
	const unsigned char* base; size_t stride; int count, compType, components; bool norm;
	accessorMeta(root, bin, idx, base, stride, count, compType, components, norm);
	std::vector<float> out;
	out.reserve(size_t(count * components));

	for (int i = 0; i < count; i++) {
		const unsigned char* p = base + size_t(i) * stride;
		for (int c = 0; c < components; c++) out.push_back(readFloat(p + size_t(c * componentByteSize(compType)), compType, norm));
	}
	return out;
}

std::vector<unsigned int> readIndexAccessor(const json& root, const std::vector<unsigned char>& bin, int idx) {
	const unsigned char* base; size_t stride; int count, compType, components; bool norm;
	accessorMeta(root, bin, idx, base, stride, count, compType, components, norm);
	std::vector<unsigned int> out;
	out.reserve(size_t(count));

	for (int i = 0; i < count; i++) out.push_back(readU32(base + size_t(i) * stride, compType));
	return out;
}

glm::vec4 baseColorFactor(const json& root, int materialIndex) {
	if (materialIndex < 0 || !root.contains("materials")) return glm::vec4(1.f);
	const auto& mats = root.at("materials");

	if (materialIndex >= int(mats.size())) return glm::vec4(1.f);
	const json& mat = mats[materialIndex];

	if (mat.contains("pbrMetallicRoughness") && mat["pbrMetallicRoughness"].contains("baseColorFactor")) {
		auto a = mat["pbrMetallicRoughness"]["baseColorFactor"];
		return glm::vec4(
			a.size() > 0 ? a[0].get<float>() : 1.f,
			a.size() > 1 ? a[1].get<float>() : 1.f,
			a.size() > 2 ? a[2].get<float>() : 1.f,
			a.size() > 3 ? a[3].get<float>() : 1.f
		);
	}

	return glm::vec4(1.f);
}

void applyNodeTransform(Object3D& obj, const json& node) {
	if (node.contains("matrix") && node["matrix"].size() == 16) {
		float e[16];

		for (int i = 0; i < 16; i++) e[i] = node["matrix"][i].get<float>();

		glm::mat4 mat = glm::make_mat4(e);
		glm::vec3 skew; glm::vec4 persp;
		glm::decompose(mat, obj.scale, obj.quaternion, obj.position, skew, persp);
		obj.quaternion = glm::normalize(obj.quaternion);
	} else {
		if (node.contains("translation")) {
			auto t = node["translation"];
			obj.position = {t[0].get<float>(), t[1].get<float>(), t[2].get<float>()};
		}

		if (node.contains("rotation")) {
			auto r = node["rotation"]; // xyzw
			obj.quaternion = glm::normalize(glm::quat(r[3].get<float>(), r[0].get<float>(), r[1].get<float>(), r[2].get<float>()));
		}

		if (node.contains("scale")) {
			auto s = node["scale"];
			obj.scale = {s[0].get<float>(), s[1].get<float>(), s[2].get<float>()};
		}
	}

	obj.updateMatrix();
}

void applyDefaultMorphWeights(Mesh& mesh, const json& meshDef, const json& node) {
	size_t nTargets = 0;
	if (mesh.geometry) {
		auto it = mesh.geometry->morphAttributes.find("position");
		if (it != mesh.geometry->morphAttributes.end()) nTargets = it->second.size();
	}

	if (nTargets == 0) return;
	mesh.morphTargetInfluences.assign(nTargets, 0.0f);

	if (node.contains("weights") && node["weights"].is_array()) {
		for (size_t i = 0; i < nTargets && i < node["weights"].size(); i++) mesh.morphTargetInfluences[i] = node["weights"][i].get<float>();
	} else if (meshDef.contains("weights") && meshDef["weights"].is_array()) {
		for (size_t i = 0; i < nTargets && i < meshDef["weights"].size(); i++) mesh.morphTargetInfluences[i] = meshDef["weights"][i].get<float>();
	}

	mesh.setMorphInfluences(mesh.morphTargetInfluences);
}

std::shared_ptr<BufferGeometry> buildGeometry(const json& root, const std::vector<unsigned char>& bin, const json& prim) {
	auto geo = std::make_shared<BufferGeometry>();
	const json& attrs = prim.at("attributes");

	if (attrs.contains("POSITION"))   geo->setAttribute("position", BufferAttribute(readFloatAccessor(root, bin, attrs["POSITION"].get<int>()), 3));
	if (attrs.contains("NORMAL"))     geo->setAttribute("normal", BufferAttribute(readFloatAccessor(root, bin, attrs["NORMAL"].get<int>()), 3));
	if (attrs.contains("TEXCOORD_0")) geo->setAttribute("uv", BufferAttribute(readFloatAccessor(root, bin, attrs["TEXCOORD_0"].get<int>()), 2));
	if (attrs.contains("JOINTS_0"))   geo->setAttribute("joints", BufferAttribute(readFloatAccessor(root, bin, attrs["JOINTS_0"].get<int>()), 4));
	if (attrs.contains("WEIGHTS_0"))  geo->setAttribute("weights", BufferAttribute(readFloatAccessor(root, bin, attrs["WEIGHTS_0"].get<int>()), 4));

	if (prim.contains("target") && prim["targets"].is_array()) {
		geo->morphTargetsRelative = true;
		for (const auto& target : prim["target"]) {
			if (target.contains("POSITION")) {
				auto data = readFloatAccessor(root, bin, target["POSITION"].get<int>());
				geo->morphAttributes["position"].push_back(BufferAttribute(std::move(data), 3));
			}

			if (target.contains("NORMAL")) {
				auto data = readFloatAccessor(root, bin, target["NORMAL"].get<int>());
				geo->morphAttributes["normal"].push_back(BufferAttribute(std::move(data), 3));
			}
		}
	}

	if (prim.contains("indices"))     geo->setIndex(readIndexAccessor(root, bin, prim["indices"].get<int>()));
	if (!geo->hasAttribute("normal") && geo->hasAttribute("position")) geo->computeVertexNormals();

	geo->computeBoundingBox();
	geo->computeBoundingSphere();
	return geo;
}

std::vector<std::shared_ptr<Texture>> loadTexture(const json& root, const std::vector<unsigned char>& bin) {
	std::vector<std::shared_ptr<Texture>> textures;
	if (!root.contains("textures") || !root.contains("images")) return textures;

	const auto& images = root["images"];
	const auto& texDefs = root["textures"];
	textures.resize(texDefs.size());

	for (size_t ti = 0; ti < texDefs.size(); ++ti) {
		const json& tex = texDefs[ti];
		if (!tex.contains("source")) continue;

		int imageIndex = tex["source"].get<int>();
		if (imageIndex < 0 || imageIndex >= int(images.size())) continue;
		const json& image = images[imageIndex];

		try {
			if (image.contains("bufferView")) {
				int bvIndex = image["bufferView"].get<int>();
				const json& bv = root.at("bufferViews").at(bvIndex);

				int offset = bv.value("byteOffset", 0);
				int length = bv.at("byteLength").get<int>();
				if (offset < 0 || length <= 0 || size_t(offset + length) > bin.size()) continue;
				Texture raw = TextureUtil::loadFromMemory(bin.data() + offset, length, false);

				auto ptr = std::make_shared<Texture>(std::move(raw));
				ptr->bind();

				GLint wrapS = GL_REPEAT;
				GLint wrapT = GL_REPEAT;
				GLint minFilter = GL_LINEAR;
				GLint magFilter = GL_LINEAR;
				bool needMipmaps = false;

				if (tex.contains("sampler") && root.contains("samplers")) {
					int si = tex["sampler"].get<int>();
					if (si >= 0 && si < int(root["samplers"].size())) {
						const json& samp = root["samplers"][si];

						if (samp.contains("wrapS")) wrapS = samp["wrapS"].get<int>();
						if (samp.contains("wrapT")) wrapT = samp["wrapT"].get<int>();
						if (samp.contains("magFilter")) magFilter = samp["magFilter"].get<int>();
						if (samp.contains("minFilter")) {
							minFilter = samp["minFilter"].get<int>();
							needMipmaps = (minFilter == GL_NEAREST_MIPMAP_NEAREST || minFilter == GL_LINEAR_MIPMAP_NEAREST || minFilter == GL_NEAREST_MIPMAP_LINEAR || minFilter == GL_LINEAR_MIPMAP_LINEAR || minFilter == 9984 || minFilter == 9985 || minFilter == 9986 || minFilter == 9987);
						}
					}
				}

				ptr->setParameter(GL_TEXTURE_WRAP_S, wrapS);
				ptr->setParameter(GL_TEXTURE_WRAP_T, wrapT);
				ptr->setParameter(GL_TEXTURE_MAG_FILTER, magFilter);

				if (needMipmaps) {
					glGenerateMipmap(GL_TEXTURE_2D);
				} else if (minFilter == 9984 || minFilter == 9985 || minFilter == 9986 || minFilter == 9987) {
					glGenerateMipmap(GL_TEXTURE_2D);
					needMipmaps = true;
				}

				ptr->setParameter(GL_TEXTURE_MIN_FILTER, minFilter);
				ptr->unbind();
				textures[ti] = ptr;
			}
		} catch (const std::exception& e) {
			(void)e;
		}
	}

	return textures;
}

std::shared_ptr<Material> buildMaterial(const json& root, int materialIndex, const std::vector<std::shared_ptr<Material>>& materials, const std::vector<std::shared_ptr<Texture>>& textures, const std::shared_ptr<Material>& defaultMat) {
	if (materialIndex < 0 || materialIndex >= int(materials.size())) return defaultMat;
	return materials[materialIndex];
}

} // namespace

void GLTFModel::adoptInto(Scene& scene) {
	if (!root) return;
	Object3D* rootPtr = root;
	for (auto& node : ownedNodes) {
		if (!node) continue;

		if (node.get() == rootPtr) {
			scene.adopt(std::move(node));
		} else {
			scene.own(std::move(node));
		}
	}

	ownedNodes.clear();
	root = rootPtr;
}

GLTFModel GLTFLoader::load(const std::string& path) {
	std::ifstream file(path, std::ios::binary | std::ios::ate);
	if (!file) throw std::runtime_error("GLTFLoader: cannot open " + path);

	auto size = file.tellg();
	file.seekg(0);
	std::vector<unsigned char> buf((size_t(size)));

	if (!file.read(reinterpret_cast<char*>(buf.data()), size)) throw std::runtime_error("GLTFLoader: failed to read " + path);
	auto slash = path.find_last_of("/\\");

	if (slash != std::string::npos) this->basePath = path.substr(0, slash + 1);
	std::string name = slash != std::string::npos ? path.substr(slash + 1) : path;

	return this->loadFromMemory(buf.data(), buf.size(), name);
}

GLTFModel GLTFLoader::loadFromMemory(const unsigned char* data, size_t size, const std::string& name) {
	ParsedGLB glb = parseGLB(data, size);
	const json& root = glb.root;
	const auto& bin = glb.bin;

	GLTFModel model;
	model.name = name;

	// Textures
	std::vector<std::shared_ptr<Texture>> textures = loadTexture(root, bin);
	model.textures = textures;

	// Materials
	std::vector<std::shared_ptr<Material>> materials;
	if (root.contains("materials")) {
		for (size_t i = 0; i < root["materials"].size(); i++) {
			auto mat = std::make_shared<Material>();
			mat->color = baseColorFactor(root, int(i));

			if (root["materials"][i].contains("name")) mat->name = root["materials"][i]["name"].get<std::string>();
			if (root["materials"][i].value("doubleSided", false)) mat->side = 2;
			if (root["materials"][i].value("alphaMode", "OPAQUE") == "BLEND") mat->transparent = true;

			const json& matJson = root["materials"][i];
			if (matJson.contains("pbrMetallicRoughness") && matJson["pbrMetallicRoughness"].contains("baseColorTexture")) {
				int texIndex = matJson["pbrMetallicRoughness"]["baseColorTexture"].value("index", -1);
				if (texIndex >= 0 && texIndex < int(textures.size()) && textures[texIndex]) mat->map = textures[texIndex];
			}

			materials.push_back(mat);
			model.materials.push_back(mat);
		}
	}
	auto defaultMat = std::make_shared<Material>(glm::vec4(0.75f, 0.75f, 0.78f, 1.f));
	if (materials.empty()) {
		materials.push_back(defaultMat);
		model.materials.push_back(defaultMat);
	}

	// Create nodes
	std::vector<Object3D*> nodePtrs;
	if (root.contains("nodes")) {
		const auto& nodes = root["nodes"];
		nodePtrs.resize(nodes.size(), nullptr);

		for (size_t i = 0; i < nodes.size(); i++) {
			const json& node = nodes[i];
			std::unique_ptr<Object3D> obj;

			if (node.contains("mesh")) {
				int meshIndex = node["mesh"].get<int>();
				const json& meshDef = root.at("meshes").at(meshIndex);
				const auto& prims = meshDef.at("primitives");

				if (prims.size() == 1) {
					// Single primitive → node is the Mesh itself
					auto geo = buildGeometry(root, bin, prims[0]);
					model.geometries.push_back(geo);

					int mi = prims[0].value("material", -1);
					auto mat = (mi >= 0 && mi < int(materials.size())) ? materials[mi] : defaultMat;
					auto mesh = std::make_unique<Mesh>(geo, mat);

					if (node.contains("name")) mesh->name = node["name"].get<std::string>();
					else if (meshDef.contains("name")) mesh->name = meshDef["name"].get<std::string>();

					applyDefaultMorphWeights(*mesh, meshDef, node);
					applyNodeTransform(*mesh, node);
					nodePtrs[i] = mesh.get();
					obj = std::move(mesh);
				} else {
					// Multiple primitives → empty parent + child meshes
					obj = std::make_unique<Object3D>();

					if (node.contains("name")) obj->name = node["name"].get<std::string>();
					applyNodeTransform(*obj, node);
					nodePtrs[i] = obj.get();

					for (const auto& prim : prims) {
						auto geo = buildGeometry(root, bin, prim);
						model.geometries.push_back(geo);

						int mi = prim.value("material", -1);
						auto mat = (mi >= 0 && mi < int(materials.size())) ? materials[mi] : defaultMat;
						auto mesh = std::make_unique<Mesh>(geo, mat);

						if (meshDef.contains("name")) mesh->name = meshDef["name"].get<std::string>();
						Object3D* meshPtr = mesh.get();
						model.ownedNodes.push_back(std::move(mesh));
						obj->add(meshPtr);
					}
				}
			} else {
				obj = std::make_unique<Object3D>();

				if (node.contains("name")) obj->name = node["name"].get<std::string>();
				applyNodeTransform(*obj, node);
				nodePtrs[i] = obj.get();
			}

			model.ownedNodes.push_back(std::move(obj));
		}

		// Wire children
		for (size_t i = 0; i < nodes.size(); i++) {
			if (!nodes[i].contains("children")) continue;
			for (const auto& childIdx : nodes[i]["children"]) {
				int ci = childIdx.get<int>();

				if (ci >= 0 && ci < int(nodePtrs.size()) && nodePtrs[i] && nodePtrs[ci]) {
					nodePtrs[i]->add(nodePtrs[ci]);
				}
			}
		}

		// Scene roots
		auto rootObj = std::make_unique<Object3D>();
		rootObj->name = name.empty() ? "GLTFScene" : name;
		model.root = rootObj.get();

		if (root.contains("scenes") && root.contains("scene")) {
			int sceneIndex = root["scene"].get<int>();
			const json& scn = root["scenes"].at(sceneIndex);

			if (scn.contains("nodes")) {
				for (const auto& ni : scn["nodes"]) {
					int idx = ni.get<int>();
					if (idx >= 0 && idx < int(nodePtrs.size()) && nodePtrs[idx]) rootObj->add(nodePtrs[idx]);
				}
			}
		} else if (!nodePtrs.empty()) {
			// Fallback: nodes with no parent
			std::vector<bool> hasParent(nodePtrs.size(), false);
			for (size_t i = 0; i < nodes.size(); i++) {
				if (!nodes[i].contains("children")) continue;
				for (const auto& c : nodes[i]["children"]) {
					int ci = c.get<int>();
					if (ci >= 0 && ci < int(hasParent.size())) hasParent[ci] = true;
				}
			}
			for (size_t i = 0; i < nodePtrs.size(); i++) {
				if (nodePtrs[i] && !hasParent[i]) rootObj->add(nodePtrs[i]);
			}
		}

		model.ownedNodes.push_back(std::move(rootObj));
	} else if (root.contains("meshes")) {
		// No nodes — create a mesh per primitive under root
		auto rootObj = std::make_unique<Object3D>();
		rootObj->name = name.empty() ? "GLTFScene" : name;
		model.root = rootObj.get();
		for (size_t mi = 0; mi < root["meshes"].size(); mi++) {
			const json& meshDef = root["meshes"][mi];

			for (const auto& prim : meshDef.at("primitives")) {
				auto geo = buildGeometry(root, bin, prim);
				model.geometries.push_back(geo);

				int midx = prim.value("material", -1);
				auto mat = (midx >= 0 && midx < int(materials.size())) ? materials[midx] : defaultMat;
				auto mesh = std::make_unique<Mesh>(geo, mat);
				
				if (meshDef.contains("name")) mesh->name = meshDef["name"].get<std::string>();
				Object3D* ptr = mesh.get();
				model.ownedNodes.push_back(std::move(mesh));
				rootObj->add(ptr);
			}
		}
		model.ownedNodes.push_back(std::move(rootObj));
	}

	if (root.contains("skins") && !nodePtrs.empty()) {
		for (size_t si = 0; si < root["skins"].size(); si++) {
			const json& skinDef = root["skins"][si];
			auto skin = std::make_shared<Skin>();
			if (skinDef.contains("name")) skin->name = skinDef["name"].get<std::string>();

			std::vector<int> jointIndices;
			if (skinDef.contains("joints")) {
				for (const auto& j : skinDef["joints"]) jointIndices.push_back(j.get<int>());
			}
			skin->resize(jointIndices.size());
			for (size_t ji = 0; ji < jointIndices.size(); ji++) {
				int ni = jointIndices[ji];
				if (ni >= 0 && ni < int(nodePtrs.size())) skin->joints[ji] = nodePtrs[ni];
			}

			if (skinDef.contains("inverseBindMatrices")) {
				auto ibm = readFloatAccessor(root, bin, skinDef["inverseBindMatrices"].get<int>());
				// 16 floats per matrix
				size_t matCount = ibm.size() / 16;
				for (size_t mi = 0; mi < matCount && mi < skin->inverseBindMatrices.size(); mi++) {
					const float* m = ibm.data() + mi * 16;
					skin->inverseBindMatrices[mi] = glm::make_mat4(ibm.data() + mi * 16);
				}
			}

			if (skinDef.contains("skeleton")) {
				int sk = skinDef["skeleton"].get<int>();
				if (sk >= 0 && sk < int(nodePtrs.size())) skin->skeleton = nodePtrs[sk];
			}

			model.skins.push_back(skin);
		}

		// Assign skins to meshes: node.skin index → mesh.skin
		for (size_t i = 0; i < root["nodes"].size(); i++) {
			const json& node = root["nodes"][i];
			if (!node.contains("skin") || !node.contains("mesh")) continue;

			int skinIndex = node["skin"].get<int>();
			if (skinIndex < 0 || skinIndex >= int(model.skins.size())) continue;

			auto skin = model.skins[skinIndex];
			auto assignSkin = [&](Object3D* obj) {
				if (auto* mesh = dynamic_cast<Mesh*>(obj)) {
					mesh->skin = skin;
					// Re-upload so joint/weight attributes are bound if geometry has them
					if (mesh->geometry) mesh->upload();
				}
				for (Object3D* child : obj->children) {
					if (auto* mesh = dynamic_cast<Mesh*>(child)) {
						mesh->skin = skin;
						if (mesh->geometry) mesh->upload();
					}
				}
			};
			if (nodePtrs[i]) assignSkin(nodePtrs[i]);
		}
	}

	// Animations
	if (root.contains("animations") && !nodePtrs.empty()) {
		for (const auto& animDef : root["animations"]) {
			AnimationClip clip;
			if (animDef.contains("name")) clip.name = animDef["name"].get<std::string>();

			const auto& samplers = animDef.at("samplers");
			const auto& channels = animDef.at("channels");

			for (const auto& ch : channels) {
				if (!ch.contains("sampler") || !ch.contains("target")) continue;
				int samplerIndex = ch["sampler"].get<int>();
				if (samplerIndex < 0 || samplerIndex >= int(samplers.size())) continue;

				const json& target = ch["target"];
				if (!target.contains("node") || !target.contains("path")) continue;
				int nodeIndex = target["node"].get<int>();
				if (nodeIndex < 0 || nodeIndex >= int(nodePtrs.size()) || !nodePtrs[nodeIndex]) continue;

				const json& samp = samplers[samplerIndex];
				AnimationTrack track;
				track.target = nodePtrs[nodeIndex];

				std::string path = target["path"].get<std::string>();
				if (path == "translation") track.path = AnimationPath::Translation;
				else if (path == "rotation") track.path = AnimationPath::Rotation;
				else if (path == "scale") track.path = AnimationPath::Scale;
				else if (path == "weights") track.path = AnimationPath::Weights;
				else continue;

				std::string interp = samp.value("interpolation", "LINEAR");
				if (interp == "STEP") track.interpolation = AnimationInterpolation::Step;
				else if (interp == "CUBICSPLINE") track.interpolation = AnimationInterpolation::CubicSpline;
				else track.interpolation = AnimationInterpolation::Linear;

				track.times = readFloatAccessor(root, bin, samp.at("input").get<int>());
				track.values = readFloatAccessor(root, bin, samp.at("output").get<int>());

				// CUBICSPLINE stores 3x values per key (in-tangent, value, out-tangent) — extract middle for now
				if (track.interpolation == AnimationInterpolation::CubicSpline && !track.times.empty()) {
					size_t keys = track.times.size();

					int comps;
					if (track.path == AnimationPath::Rotation) comps =  4;
					else if (track.path == AnimationPath::Weights) comps = (keys > 0) ? static_cast<int>(track.values.size() / (keys * 3)) : 0;
					else comps = 3;

					if (comps > 0 && track.values.size() >= keys * size_t(comps) * 3) {
						size_t keys = track.times.size();
						std::vector<float> extracted;
						extracted.reserve(keys * size_t(comps));

						for (size_t k = 0; k < keys; k++) {
							size_t base = k * size_t(comps) * 3 + size_t(comps); // skip in-tangent
							for (int c = 0; c < comps; c++) extracted.push_back(track.values[base + c]);
						}
						track.values = std::move(extracted);
						track.interpolation = AnimationInterpolation::Linear; // approximate
					}
				}

				if (!track.times.empty()) {
					clip.duration = std::max(clip.duration, track.times.back());
				}
				clip.tracks.push_back(std::move(track));
			}

			if (!clip.tracks.empty()) {
				model.animations.push_back(std::move(clip));
			}
		}
	}

	if (model.root) model.root->updateMatrixWorld(true);
	return model;
}

} // namespace blaze::lightEngine
