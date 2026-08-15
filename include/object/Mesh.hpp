#pragma once

#include <object/BufferGeometry.hpp>
#include <object/Material.hpp>
#include <object/Object3D.hpp>

#include <gl/glad.h>
#include <memory>

namespace blaze::lightEngine {

class Mesh : public Object3D {
public:
    std::shared_ptr<BufferGeometry> geometry;
    std::shared_ptr<Material> material;

    Mesh() = default;
    Mesh(std::shared_ptr<BufferGeometry> geometry, std::shared_ptr<Material> material);
    ~Mesh() override;

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    void upload();
    void draw() const;
    void dispose();

    bool isUploaded() const { return uploaded; }

private:
    GLuint vao = 0;
    GLuint vboPosition = 0;
    GLuint vboNormal = 0;
    GLuint vboUv = 0;
    GLuint ebo = 0;
    GLsizei indexCount = 0;
    GLsizei vertexCount = 0;
    bool hasIndex = false;
    bool hasNormal = false;
    bool hasUv = false;
    bool uploaded = false;

    void createBuffers();
    void destroyBuffers();
};

} // namespace blaze::lightEngine
