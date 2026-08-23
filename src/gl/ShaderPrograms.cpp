#include <gl/ShaderLoader.hpp>
#include <gl/ShaderProgram.hpp>
#include <gl/ShaderPrograms.hpp>

namespace blaze::lightEngine {

std::vector<ShaderProgram*> ShaderPrograms::ALL;

std::shared_ptr<ShaderProgram> ShaderPrograms::load(std::string id) {
	ShaderProgram shader = ShaderLoader::getShaderById(id);
	auto up = std::make_shared<ShaderProgram>(std::move(shader));
	ALL.push_back(up.get());
	return up;
}

void ShaderPrograms::init() {
	if (POSITION != nullptr) return;

	ShaderPrograms::MESH               = ShaderPrograms::load("mesh");
	ShaderPrograms::POSITION           = ShaderPrograms::load("position");
	ShaderPrograms::POSITION_TEX       = ShaderPrograms::load("position_tex");
	ShaderPrograms::POSITION_COLOR     = ShaderPrograms::load("position_color");
	ShaderPrograms::POSITION_TEX_COLOR = ShaderPrograms::load("position_tex_color");
	ShaderPrograms::POSITION_TEX_ALPHA = ShaderPrograms::load("position_tex_alpha");
	ShaderPrograms::PROJECTION         = ShaderPrograms::load("projection");
}

std::shared_ptr<ShaderProgram> ShaderPrograms::MESH                = nullptr;
std::shared_ptr<ShaderProgram> ShaderPrograms::POSITION           = nullptr;
std::shared_ptr<ShaderProgram> ShaderPrograms::POSITION_COLOR     = nullptr;
std::shared_ptr<ShaderProgram> ShaderPrograms::POSITION_TEX       = nullptr;
std::shared_ptr<ShaderProgram> ShaderPrograms::POSITION_TEX_COLOR = nullptr;
std::shared_ptr<ShaderProgram> ShaderPrograms::POSITION_TEX_ALPHA = nullptr;
std::shared_ptr<ShaderProgram> ShaderPrograms::PROJECTION         = nullptr;

} // namespace blaze::lightEngine
