#pragma once

#include <object/BufferAttribute.hpp>

#include <glm/glm.hpp>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace blaze::lightEngine {

struct GeometryGroup {
    int start = 0;
    int count = 0;
    int materialIndex = 0;
};

struct DrawRange {
    int start = 0;
    int count = -1;
};

class BufferGeometry {
public:
    std::string name;
    std::string uuid;

    std::vector<unsigned int> index;
    std::unordered_map<std::string, BufferAttribute> attributes;
    std::vector<GeometryGroup> groups;
    DrawRange drawRange;

    std::optional<glm::vec3> boundingBoxMin;
    std::optional<glm::vec3> boundingBoxMax;
    std::optional<glm::vec3> boundingSphereCenter;
    std::optional<float> boundingSphereRadius;

    BufferGeometry();
    ~BufferGeometry() = default;

    BufferGeometry& setIndex(std::vector<unsigned int> indices);
    const std::vector<unsigned int>& getIndex() const { return index; }
    bool hasIndex() const { return !index.empty(); }

    BufferAttribute* getAttribute(const std::string& name);
    const BufferAttribute* getAttribute(const std::string& name) const;
    BufferGeometry& setAttribute(const std::string& name, BufferAttribute attribute);
    BufferGeometry& deleteAttribute(const std::string& name);
    bool hasAttribute(const std::string& name) const;

    void addGroup(int start, int count, int materialIndex = 0);
    void clearGroups();
    void setDrawRange(int start, int count);

    BufferGeometry& applyMatrix4(const glm::mat4& matrix);
    BufferGeometry& rotateX(float angle);
    BufferGeometry& rotateY(float angle);
    BufferGeometry& rotateZ(float angle);
    BufferGeometry& translate(float x, float y, float z);
    BufferGeometry& scale(float x, float y, float z);

    void computeBoundingBox();
    void computeBoundingSphere();
    void computeVertexNormals();
    int vertexCount() const;
    void dispose();

    static BufferGeometry box(float width = 1.0f, float height = 1.0f, float depth = 1.0f);
    static BufferGeometry plane(float width = 1.0f, float height = 1.0f);
    static BufferGeometry sphere(float radius = 0.5f, int widthSegments = 32, int heightSegments = 16);

private:
    static int nextId;
};

} // namespace blaze::lightEngine
