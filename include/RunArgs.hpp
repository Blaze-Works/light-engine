#pragma once

#include <string>
#include <vector>

#include <WindowSettings.hpp>

namespace blaze::lightEngine {

struct DirectorySet {
	std::string runDir;
	std::string resourceDir;
	std::string assetDir;
	std::string assetIndex;

	DirectorySet(
		std::string workingDirectory = "..",
		std::string resourcesDirectory = "../resources/",
		std::string absoluteResourcesDirectory = "../resources/"
	);
};

struct RunArgs {
	WindowSettings windowSettings;
	DirectorySet directories;
	bool useDebug;
	int overrideWidth;
	int overrideHeight;
	std::vector<std::string> arguments;

	RunArgs(
		WindowSettings windowSettings = WindowSettings(),
		DirectorySet directories = DirectorySet(),
		bool useDebug = false,
		int overrideWidth = 0,
		int overrideHeight = 0,
		std::vector<std::string> arguments = {}
	);
};

} // namespace blaze::lightEngine

