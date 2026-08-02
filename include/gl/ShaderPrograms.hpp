#pragma once

#include "ShaderProgram.hpp"

#include <memory>
#include <string>
#include <vector>

namespace blaze::lightEngine {

struct ShaderPrograms {
    static std::unique_ptr<ShaderProgram> load(std::string id);
    static void init();

    static std::vector<ShaderProgram*> ALL;
    static std::unique_ptr<ShaderProgram> POSITION;
    static std::unique_ptr<ShaderProgram> POSITION_COLOR;
    static std::unique_ptr<ShaderProgram> POSITION_TEX;
    static std::unique_ptr<ShaderProgram> POSITION_TEX_COLOR;
    static std::unique_ptr<ShaderProgram> POSITION_TEX_ALPHA;
    static std::unique_ptr<ShaderProgram> PROJECTION;
    static std::unique_ptr<ShaderProgram> PROJECTION_INDICES;
};

}
