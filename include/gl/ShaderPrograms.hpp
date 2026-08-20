#pragma once

#include <gl/ShaderProgram.hpp>

#include <memory>
#include <string>
#include <vector>

namespace blaze::lightEngine {

struct ShaderPrograms {
	static std::shared_ptr<ShaderProgram> load(std::string id);
	static void init();

	static std::vector<ShaderProgram*> ALL;
	static std::shared_ptr<ShaderProgram> POSITION;
	static std::shared_ptr<ShaderProgram> POSITION_COLOR;
	static std::shared_ptr<ShaderProgram> POSITION_TEX;
	static std::shared_ptr<ShaderProgram> POSITION_TEX_COLOR;
	static std::shared_ptr<ShaderProgram> POSITION_TEX_ALPHA;
	static std::shared_ptr<ShaderProgram> PROJECTION;
	static std::shared_ptr<ShaderProgram> PROJECTION_INDICES;
};

}
