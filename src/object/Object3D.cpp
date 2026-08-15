#include <object/Object3D.hpp>
#include <util/math/MathUtils.hpp>

#include <glm/gtc/matrix_inverse.hpp>

namespace blaze::lightEngine {

glm::vec3 Object3D::DefaultUp{0.0f, 1.0f, 0.0f};
bool Object3D::DefaultMatrixAutoUpdate = true;
int Object3D::nextId = 0;

Object3D::Object3D(): id(nextId++), uuid(MathUtils::generateUUID()), up(DefaultUp), matrixAutoUpdate(DefaultMatrixAutoUpdate){}

void Object3D::applyMatrix4(const glm::mat4& m) {
    if (matrixAutoUpdate) updateMatrix();
    matrix = m * matrix;

    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(matrix, scale, quaternion, position, skew, perspective);
    quaternion = glm::normalize(quaternion);
    matrixWorldNeedsUpdate = true;
}

Object3D& Object3D::applyQuaternion(const glm::quat& q) {
    quaternion = glm::normalize(q * quaternion);
    matrixWorldNeedsUpdate = true;
    return *this;
}

void Object3D::setRotationFromAxisAngle(const glm::vec3& axis, float angle) {
    quaternion = glm::angleAxis(angle, glm::normalize(axis));
    matrixWorldNeedsUpdate = true;
}

void Object3D::setRotationFromQuaternion(const glm::quat& q) {
    quaternion = glm::normalize(q);
    matrixWorldNeedsUpdate = true;
}

void Object3D::setRotationFromMatrix(const glm::mat4& m) {
    quaternion = glm::normalize(glm::quat_cast(m));
    matrixWorldNeedsUpdate = true;
}

Object3D& Object3D::rotateOnAxis(const glm::vec3& axis, float angle) {
    glm::quat q = glm::angleAxis(angle, glm::normalize(axis));
    quaternion = glm::normalize(quaternion * q);
    matrixWorldNeedsUpdate = true;
    return *this;
}

Object3D& Object3D::rotateOnWorldAxis(const glm::vec3& axis, float angle) {
    glm::quat q = glm::angleAxis(angle, glm::normalize(axis));
    quaternion = glm::normalize(q * quaternion);
    matrixWorldNeedsUpdate = true;
    return *this;
}

Object3D& Object3D::rotateX(float angle) {
    return rotateOnAxis(glm::vec3(1, 0, 0), angle);
}

Object3D& Object3D::rotateY(float angle) {
    return rotateOnAxis(glm::vec3(0, 1, 0), angle);
}

Object3D& Object3D::rotateZ(float angle) {
    return rotateOnAxis(glm::vec3(0, 0, 1), angle);
}

Object3D& Object3D::translateOnAxis(const glm::vec3& axis, float distance) {
    glm::vec3 v = glm::normalize(axis) * distance;
    v = quaternion * v;
    position += v;
    matrixWorldNeedsUpdate = true;
    return *this;
}

Object3D& Object3D::translateX(float distance) {
    return translateOnAxis(glm::vec3(1, 0, 0), distance);
}

Object3D& Object3D::translateY(float distance) {
    return translateOnAxis(glm::vec3(0, 1, 0), distance);
}

Object3D& Object3D::translateZ(float distance) {
    return translateOnAxis(glm::vec3(0, 0, 1), distance);
}

glm::vec3 Object3D::localToWorld(const glm::vec3& v) const {
    return glm::vec3(matrixWorld * glm::vec4(v, 1.0f));
}

glm::vec3 Object3D::worldToLocal(const glm::vec3& v) const {
    glm::mat4 inv = glm::inverse(matrixWorld);
    return glm::vec3(inv * glm::vec4(v, 1.0f));
}

void Object3D::lookAt(const glm::vec3& target) {
    lookAt(target.x, target.y, target.z);
}

// Kinda broken
void Object3D::lookAt(float x, float y, float z) {
    glm::vec3 target(x, y, z);
    updateWorldMatrix(true, false);
    glm::vec3 eye = glm::vec3(matrixWorld[3]);

    glm::mat4 m(1.0f);
    if (isCameraType) {
        m = glm::lookAt(eye, target, up);
    } else {
        m = glm::lookAt(target, eye, up);
    }

    quaternion = glm::normalize(glm::quat_cast(m));

    if (parent) {
        parent->updateWorldMatrix(true, false);
        glm::quat parentRot = glm::normalize(glm::quat_cast(parent->matrixWorld));
        quaternion = glm::normalize(glm::inverse(parentRot) * quaternion);
    }
    matrixWorldNeedsUpdate = true;
}

Object3D& Object3D::add(Object3D* object) {
    if (!object || object == this) return *this;

    if (object->parent) {
        object->parent->remove(object);
    }

    object->parent = this;
    children.push_back(object);
    return *this;
}

Object3D& Object3D::remove(Object3D* object) {
    if (!object) return *this;

    auto it = std::find(children.begin(), children.end(), object);
    if (it != children.end()) {
        object->parent = nullptr;
        children.erase(it);
    }
    return *this;
}

Object3D& Object3D::clear() {
    for (Object3D* child : children) {
        child->parent = nullptr;
    }
    children.clear();
    return *this;
}

Object3D& Object3D::attach(Object3D* object) {
    if (!object) return *this;

    updateWorldMatrix(true, false);
    glm::mat4 invParentWorld = glm::inverse(matrixWorld);

    if (object->parent) {
        object->parent->updateWorldMatrix(true, false);
        invParentWorld = invParentWorld * object->parent->matrixWorld;
    }

    object->applyMatrix4(invParentWorld);
    object->updateWorldMatrix(false, false);
    add(object);
    return *this;
}

Object3D* Object3D::getObjectById(int searchId) {
    if (id == searchId) return this;
    for (Object3D* child : children) {
        if (Object3D* found = child->getObjectById(searchId)) return found;
    }
    return nullptr;
}

Object3D* Object3D::getObjectByName(const std::string& searchName) {
    if (name == searchName) return this;
    for (Object3D* child : children) {
        if (Object3D* found = child->getObjectByName(searchName)) return found;
    }
    return nullptr;
}

glm::vec3 Object3D::getWorldPosition() const {
    const_cast<Object3D*>(this)->updateWorldMatrix(true, false);
    return glm::vec3(matrixWorld[3]);
}

glm::quat Object3D::getWorldQuaternion() const {
    const_cast<Object3D*>(this)->updateWorldMatrix(true, false);
    glm::vec3 t, s, skew;
    glm::vec4 persp;
    glm::quat q;
    glm::decompose(matrixWorld, s, q, t, skew, persp);
    return glm::normalize(q);
}

glm::vec3 Object3D::getWorldScale() const {
    const_cast<Object3D*>(this)->updateWorldMatrix(true, false);
    glm::vec3 t, s, skew;
    glm::vec4 persp;
    glm::quat q;
    glm::decompose(matrixWorld, s, q, t, skew, persp);
    return s;
}

glm::vec3 Object3D::getWorldDirection() const {
    const_cast<Object3D*>(this)->updateWorldMatrix(true, false);
    glm::vec3 dir(-matrixWorld[2][0], -matrixWorld[2][1], -matrixWorld[2][2]);
    return glm::normalize(dir);
}

void Object3D::traverse(const std::function<void(Object3D*)>& callback) {
    callback(this);
    
    std::vector<Object3D*> copy = children;
    for (Object3D* child : copy) {
        child->traverse(callback);
    }
}

void Object3D::traverseVisible(const std::function<void(Object3D*)>& callback) {
    if (!visible) return;
    callback(this);
    std::vector<Object3D*> copy = children;
    for (Object3D* child : copy) {
        child->traverseVisible(callback);
    }
}

void Object3D::traverseAncestors(const std::function<void(Object3D*)>& callback) {
    if (parent) {
        callback(parent);
        parent->traverseAncestors(callback);
    }
}

void Object3D::updateMatrix() {
    matrix = glm::translate(glm::mat4(1.0f), position) * glm::mat4_cast(quaternion) * glm::scale(glm::mat4(1.0f), scale);
    matrixWorldNeedsUpdate = true;
}

void Object3D::updateMatrixWorld(bool force) {
    if (matrixAutoUpdate) updateMatrix();

    if (matrixWorldNeedsUpdate || force) {
        if (!parent) {
            matrixWorld = matrix;
        } else {
            matrixWorld = parent->matrixWorld * matrix;
        }
        matrixWorldNeedsUpdate = false;
        force = true;
    }

    for (Object3D* child : children) {
        child->updateMatrixWorld(force);
    }
}

void Object3D::updateWorldMatrix(bool updateParents, bool updateChildren) {
    if (updateParents && parent) {
        parent->updateWorldMatrix(true, false);
    }

    if (matrixAutoUpdate) updateMatrix();

    if (!parent) {
        matrixWorld = matrix;
    } else {
        matrixWorld = parent->matrixWorld * matrix;
    }

    if (updateChildren) {
        for (Object3D* child : children) {
            child->updateWorldMatrix(false, true);
        }
    }
}

glm::vec3 Object3D::getEuler() const {
    return glm::eulerAngles(quaternion);
}

void Object3D::setEuler(const glm::vec3& eulerRadians) {
    quaternion = glm::quat(eulerRadians);
    matrixWorldNeedsUpdate = true;
}

} // namespace blaze::lightEngine
