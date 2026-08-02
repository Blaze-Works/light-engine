#include <gl/ShaderLoader.hpp>
#include <gl/ShaderProgram.hpp>
#include <gl/ShaderPrograms.hpp>

namespace blaze::lightEngine {

std::vector<ShaderProgram*> ShaderPrograms::ALL;

std::unique_ptr<ShaderProgram> ShaderPrograms::load(std::string id) {
    ShaderProgram shader = ShaderLoader::getShaderById(id);
    auto up = std::make_unique<ShaderProgram>(shader);
    ALL.push_back(up.get());
    return up;
}

void ShaderPrograms::init() {
    if (POSITION != nullptr) return;

    ShaderPrograms::POSITION           = ShaderPrograms::load("position");
    ShaderPrograms::POSITION_TEX       = ShaderPrograms::load("position_tex");
    ShaderPrograms::POSITION_COLOR     = ShaderPrograms::load("position_color");
    ShaderPrograms::POSITION_TEX_COLOR = ShaderPrograms::load("position_tex_color");
    ShaderPrograms::POSITION_TEX_ALPHA = ShaderPrograms::load("position_tex_alpha");
    ShaderPrograms::PROJECTION         = ShaderPrograms::load("projection");
    ShaderPrograms::PROJECTION_INDICES = ShaderPrograms::load("projection_indices");
}

std::unique_ptr<ShaderProgram> ShaderPrograms::POSITION           = nullptr;
std::unique_ptr<ShaderProgram> ShaderPrograms::POSITION_COLOR     = nullptr;
std::unique_ptr<ShaderProgram> ShaderPrograms::POSITION_TEX       = nullptr;
std::unique_ptr<ShaderProgram> ShaderPrograms::POSITION_TEX_COLOR = nullptr;
std::unique_ptr<ShaderProgram> ShaderPrograms::POSITION_TEX_ALPHA = nullptr;
std::unique_ptr<ShaderProgram> ShaderPrograms::PROJECTION         = nullptr;
std::unique_ptr<ShaderProgram> ShaderPrograms::PROJECTION_INDICES = nullptr;

} // namespace blaze::lightEngine
