#include <object/Mesh.hpp>

namespace blaze::lightEngine {

Mesh::Mesh(std::shared_ptr<BufferGeometry> geometry, std::shared_ptr<Material> material): geometry(std::move(geometry)), material(std::move(material)) {
    if (this->geometry) this->upload();
}

Mesh::~Mesh() {
    this->destroyBuffers();
}

void Mesh::createBuffers() {
    if (this->vao == 0) glGenVertexArrays(1, &vao);
    if (this->vboPosition == 0) glGenBuffers(1, &vboPosition);
    if (this->vboNormal == 0) glGenBuffers(1, &vboNormal);
    if (this->vboUv == 0) glGenBuffers(1, &vboUv);
    if (this->ebo == 0) glGenBuffers(1, &ebo);
}

void Mesh::destroyBuffers() {
    if (this->vao) { glDeleteVertexArrays(1, &vao); vao = 0; }
    if (this->vboPosition) { glDeleteBuffers(1, &vboPosition); vboPosition = 0; }
    if (this->vboNormal) { glDeleteBuffers(1, &vboNormal); vboNormal = 0; }
    if (this->vboUv) { glDeleteBuffers(1, &vboUv); vboUv = 0; }
    if (this->ebo) { glDeleteBuffers(1, &ebo); ebo = 0; }
    this->uploaded = false;
}

void Mesh::upload() {
    if (!geometry) return;

    this->createBuffers();
    glBindVertexArray(vao);

    const BufferAttribute* pos = geometry->getAttribute("position");
    if (pos && pos->count() > 0) {
        glBindBuffer(GL_ARRAY_BUFFER, vboPosition);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(pos->byteLength()), pos->data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, pos->itemSize, GL_FLOAT, pos->normalized ? GL_TRUE : GL_FALSE, 0, nullptr);
        vertexCount = static_cast<GLsizei>(pos->count());
    } else {
        vertexCount = 0;
    }

    const BufferAttribute* normal = geometry->getAttribute("normal");
    hasNormal = normal && normal->count() > 0;
    if (hasNormal) {
        glBindBuffer(GL_ARRAY_BUFFER, vboNormal);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(normal->byteLength()), normal->data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, normal->itemSize, GL_FLOAT, normal->normalized ? GL_TRUE : GL_FALSE, 0, nullptr);
    } else {
        glDisableVertexAttribArray(1);
    }

    const BufferAttribute* uv = geometry->getAttribute("uv");
    hasUv = uv && uv->count() > 0;
    if (hasUv) {
        glBindBuffer(GL_ARRAY_BUFFER, vboUv);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(uv->byteLength()), uv->data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, uv->itemSize, GL_FLOAT, uv->normalized ? GL_TRUE : GL_FALSE, 0, nullptr);
    } else {
        glDisableVertexAttribArray(2);
    }

    hasIndex = geometry->hasIndex();
    if (hasIndex) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(geometry->index.size() * sizeof(unsigned int)), geometry->index.data(), GL_STATIC_DRAW);
        indexCount = static_cast<GLsizei>(geometry->index.size());
    } else {
        indexCount = 0;
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    uploaded = true;
}

void Mesh::draw() const {
    if (!uploaded || vao == 0) return;

    glBindVertexArray(vao);

    GLsizei count = hasIndex ? indexCount : vertexCount;
    int start = geometry ? geometry->drawRange.start : 0;
    if (geometry && geometry->drawRange.count >= 0) {
        count = geometry->drawRange.count;
    }

    if (hasIndex) {
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

} // namespace blaze::lightEngine
