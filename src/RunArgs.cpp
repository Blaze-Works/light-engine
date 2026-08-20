#include <RunArgs.hpp>

namespace blaze::lightEngine {

DirectorySet::DirectorySet(std::string runDir, std::string resourcesDir, std::string assetDir):
	runDir(std::move(runDir)),
	resourceDir(std::move(resourcesDir)),
	assetDir(std::move(assetDir)) {}

RunArgs::RunArgs(WindowSettings windowSettings, DirectorySet directories, bool useDebug, int overrideWidth, int overrideHeight, std::vector<std::string> arguments):
	windowSettings(std::move(windowSettings)),
	directories(std::move(directories)),
	useDebug(false),
	overrideWidth(overrideWidth),
	overrideHeight(overrideHeight),
	arguments(std::move(arguments)) {}

} // namespace blaze::lightEngine

