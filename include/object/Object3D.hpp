#pragma once
#define GLM_ENABLE_EXPERIMENTAL

#include <object/Layers.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

#include <functional>
#include <string>
#include <vector>

namespace blaze::lightEngine {

class Object3D {
public:
	static glm::vec3 DefaultUp;
	static bool DefaultMatrixAutoUpdate;

	Object3D();
	Object3D(const Object3D&) = delete;
	Object3D& operator=(const Object3D&) = delete;
	virtual ~Object3D() = default;

	int id = 0;
	std::string uuid;
	std::string name;

	Object3D* parent = nullptr;
	std::vector<Object3D*> children;

	glm::vec3 up{0.0f, 1.0f, 0.0f};
	glm::vec3 position{0.0f};
	glm::quat quaternion{1.0f, 0.0f, 0.0f, 0.0f}; // w, x, y, z
	glm::vec3 scale{1.0f};

	glm::mat4 matrix{1.0f};
	glm::mat4 matrixWorld{1.0f};
	glm::mat4 modelViewMatrix{1.0f};
	glm::mat3 normalMatrix{1.0f};

	bool matrixAutoUpdate = true;
	bool matrixWorldNeedsUpdate = false;

	Layers layers;
	bool visible = true;
	bool castShadow = false;
	bool receiveShadow = false;
	bool frustumCulled = true;
	int renderOrder = 0;

	void applyMatrix4(const glm::mat4& m);
	Object3D& applyQuaternion(const glm::quat& q);

	void setRotationFromAxisAngle(const glm::vec3& axis, float angle);
	void setRotationFromQuaternion(const glm::quat& q);
	void setRotationFromMatrix(const glm::mat4& m);

	Object3D& rotateOnAxis(const glm::vec3& axis, float angle);
	Object3D& rotateOnWorldAxis(const glm::vec3& axis, float angle);
	Object3D& rotateX(float angle);
	Object3D& rotateY(float angle);
	Object3D& rotateZ(float angle);

	Object3D& translateOnAxis(const glm::vec3& axis, float distance);
	Object3D& translateX(float distance);
	Object3D& translateY(float distance);
	Object3D& translateZ(float distance);

	glm::vec3 localToWorld(const glm::vec3& v) const;
	glm::vec3 worldToLocal(const glm::vec3& v) const;

	void lookAt(const glm::vec3& target);
	void lookAt(float x, float y, float z);

	Object3D& add(Object3D* object);
	Object3D& remove(Object3D* object);
	Object3D& clear();
	Object3D& attach(Object3D* object);

	Object3D* getObjectById(int id);
	Object3D* getObjectByName(const std::string& name);

	glm::vec3 getWorldPosition() const;
	glm::quat getWorldQuaternion() const;
	glm::vec3 getWorldScale() const;
	glm::vec3 getWorldDirection() const;

	void traverse(const std::function<void(Object3D*)>& callback);
	void traverseVisible(const std::function<void(Object3D*)>& callback);
	void traverseAncestors(const std::function<void(Object3D*)>& callback);

	void updateMatrix();
	void updateMatrixWorld(bool force = false);
	void updateWorldMatrix(bool updateParents, bool updateChildren);

	glm::vec3 getEuler() const;
	void setEuler(const glm::vec3& eulerRadians);

	virtual void onBeforeRender() {}
	virtual void onAfterRender() {}

protected:
	static int nextId;

	bool isCameraType = false;

private:
	bool sameAsSnapshot();
	void updateSnapshot();

	glm::vec3 positionSnap{0.0f};
	glm::quat quaternionSnap{1.0f, 0.0f, 0.0f, 0.0f};
	glm::vec3 scaleSnap{1.0f};

};

} // namespace blaze::lightEngine
