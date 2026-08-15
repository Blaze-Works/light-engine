#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace blaze::lightEngine {

class BufferAttribute {
public:
    std::string name;
    std::vector<float> array;
    int itemSize = 3;
    bool normalized = false;
    bool needsUpdate = false;

    BufferAttribute() = default;

    BufferAttribute(std::vector<float> data, int itemSize, bool normalized = false): array(std::move(data)), itemSize(itemSize), normalized(normalized) {}

    BufferAttribute(const float* data, size_t count, int itemSize, bool normalized = false): array(data, data + count), itemSize(itemSize), normalized(normalized) {}

    int count() const {
        if (itemSize <= 0) return 0;
        return static_cast<int>(array.size()) / itemSize;
    }

    size_t byteLength() const {
        return array.size() * sizeof(float);
    }

    const float* data() const { return array.data(); }
    float* data() { return array.data(); }

    glm::vec3 getXyz(int index) const {
        int i = index * itemSize;
        return glm::vec3(array[i], array[i + 1], array.size() > static_cast<size_t>(i + 2) ? array[i + 2] : 0.0f);
    }

    void setXyz(int index, float x, float y, float z) {
        int i = index * itemSize;
        array[i] = x;
        array[i + 1] = y;
        if (itemSize >= 3) array[i + 2] = z;
        needsUpdate = true;
    }
};

} // namespace blaze::lightEngine
