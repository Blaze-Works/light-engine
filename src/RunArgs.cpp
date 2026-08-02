#include "RunArgs.hpp"

namespace blaze::lightEngine {

DirectorySet::DirectorySet(std::string runDir, std::string resourcesDir, std::string assetDir, std::string assetIndex):
      runDir(std::move(runDir)),
      resourceDir(std::move(resourcesDir)),
      assetDir(std::move(assetDir)),
      assetIndex(std::move(assetIndex)) {}

RunArgs::RunArgs(WindowSettings windowSettings, DirectorySet directories, int overrideWidth, int overrideHeight, std::vector<std::string> arguments): 
      windowSettings(std::move(windowSettings)),
      directories(std::move(directories)),
      overrideWidth(overrideWidth),
      overrideHeight(overrideHeight),
      arguments(std::move(arguments)) {}

} // namespace blaze::lightEngine

