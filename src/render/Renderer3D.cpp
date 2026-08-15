#include <color/Argb.hpp>
#include <gl/ShaderPrograms.hpp>
#include <gl/glad.h>
#include <render/Renderer3D.hpp>

#include <glm/gtc/matrix_inverse.hpp>

#include <algorithm>

namespace blaze::lightEngine {

void Renderer3D::init() {
    if (this->shader) return;
    this->setShader(ShaderPrograms::PROJECTION_INDICES);
}

void Renderer3D::setShader(std::shared_ptr<ShaderProgram> shader) {
    this->shader = shader;
}

void Renderer3D::clear(Scene* scene) {
    GLbitfield bits = 0;
    if (this->settings.autoClearColor) bits |= GL_COLOR_BUFFER_BIT;
    if (this->settings.autoClearDepth) bits |= GL_DEPTH_BUFFER_BIT;
    if (bits == 0) return;

    glm::vec4 c = settings.clearColor;
    if (scene && scene->background.has_value()) {
        int argb = *scene->background;
        c.a = Argb::getAlpha(argb) / 255.0f;
        c.r = Argb::getRed(argb) / 255.0f;
        c.g = Argb::getGreen(argb) / 255.0f;
        c.b = Argb::getBlue(argb) / 255.0f;
    }
    glClearColor(c.r, c.g, c.b, c.a);
    glClear(bits);
}

void Renderer3D::projectObject(Object3D* object, Camera& camera, std::vector<RenderItem>& list) {
    if (!object || !object->visible) return;

    if (auto* mesh = dynamic_cast<Mesh*>(object)) {
        if (mesh->isUploaded() && mesh->geometry) {
            RenderItem item;
            item.mesh = mesh;
            glm::vec4 viewPos = camera.matrixWorldInverse * glm::vec4(mesh->getWorldPosition(), 1.0f);
            item.z = viewPos.z;
            list.push_back(item);
        }
    }

    for (Object3D* child : object->children) {
        this->projectObject(child, camera, list);
    }
}

void Renderer3D::renderObject(Mesh& mesh, Camera& camera) {
    if (!this->shader) return;

    const glm::mat4& model = mesh.matrixWorld;
    glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(model)));

    this->shader->setUniform("uModel", model);
    this->shader->setUniform("uNormalMatrix", normalMat);

    glm::vec4 color(1.0f);
    bool hasTexture = false;
    GLuint texId = 0;

    if (mesh.material) {
        color = mesh.material->color;
        color.a *= mesh.material->opacity;

        if (mesh.material->map) {
            hasTexture = true;
            texId = mesh.material->map->id;
        }

        if (mesh.material->side == 1) {
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);
        } else if (mesh.material->side == 2) {
            glDisable(GL_CULL_FACE);
        } else {
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
        }

        if (mesh.material->depthTest) glEnable(GL_DEPTH_TEST);
        else glDisable(GL_DEPTH_TEST);

        glDepthMask(mesh.material->depthWrite ? GL_TRUE : GL_FALSE);

        if (mesh.material->transparent || color.a < 0.999f) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
    } else {
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
    }

    this->shader->setUniform("uColor", color);
    this->shader->setUniform("uHasTexture", hasTexture ? 1 : 0);

    if (hasTexture) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texId);
        this->shader->setUniform("uTexture", 0);
    } else {
        this->shader->setUniform("uTexture", 0);
    }

    if (mesh.material && mesh.material->wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        mesh.draw();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    } else {
        mesh.draw();
    }

    if (hasTexture) {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void Renderer3D::render(Scene& scene, Camera& camera) {
    if (!this->shader) this->init();
    if (!this->shader) return;

    if (scene.autoUpdate) scene.updateMatrixWorld(true);
    camera.updateMatrixWorld();

    if (this->settings.autoClear) this->clear(&scene);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    this->renderList.clear();
    this->projectObject(&scene, camera, this->renderList);

    if (this->settings.sortObjects) {
        std::stable_sort(this->renderList.begin(), this->renderList.end(), [](const RenderItem& a, const RenderItem& b) {
            if (a.mesh->renderOrder != b.mesh->renderOrder) return a.mesh->renderOrder < b.mesh->renderOrder;
            return a.z > b.z;
        });
    }

    this->shader->bind();
    this->shader->setUniform("uProjection", camera.projectionMatrix);
    this->shader->setUniform("uView", camera.matrixWorldInverse);

    glm::vec3 lightDir = glm::normalize(this->settings.lightDirection);
    this->shader->setUniform("uLightDir", lightDir);

    for (const RenderItem& item : this->renderList) {
        item.mesh->onBeforeRender();
        this->renderObject(*item.mesh, camera);
        item.mesh->onAfterRender();
    }

    this->shader->unbind();
    glBindVertexArray(0);
    glDisable(GL_CULL_FACE);
}

} // namespace blaze::lightEngine
