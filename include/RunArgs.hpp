#pragma once

#include <string>
#include <vector>

#include "WindowSettings.hpp"

namespace blaze::lightEngine {

struct DirectorySet {
    std::string runDir;
    std::string resourceDir;
    std::string assetDir;
    std::string assetIndex;

    DirectorySet(
        std::string workingDirectory = ".",
        std::string resourcesDirectory = "./resources/",
        std::string absoluteResourcesDirectory = "/resources/",
        std::string userDataDirectory = ""
    );
};

struct RunArgs {
    WindowSettings windowSettings;
    DirectorySet directories;
    int overrideWidth;
    int overrideHeight;
    std::vector<std::string> arguments;

    RunArgs(
        WindowSettings windowSettings,
        DirectorySet directories,
        int overrideWidth = 0,
        int overrideHeight = 0,
        std::vector<std::string> arguments = {}
    );
};

} // namespace blaze::lightEngine

