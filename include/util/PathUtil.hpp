#pragma once

#include <filesystem>
#include <fstream>
#include <vector>
#include <string>

namespace blaze::lightEngine {

namespace fs = std::filesystem;
class PathUtil {
public:
	static fs::path getPath(const fs::path& path, const std::vector<std::string>&);
	static fs::path getResourceDir();
	static void setResourceDir(const std::string& path);
	static void validatePath(const std::vector<std::string>&);
	static std::ifstream readResourceFileAsStream(const std::string& path);
	static std::string readResourceFileAsString(const std::string& path);
	static std::vector<uint8_t> readResourceFileAsBytes(const std::string& path);
	static std::vector<std::string> readResourceFileAsLines(const std::string& path);
	static std::string join(const fs::path& path1, const fs::path& path2);
	static std::string join(const std::string& path1, const std::string& path2);
	static fs::path getResourcePath(const std::string& path);
	static std::string resolveResource(const fs::path& path);

private:
	static fs::path resourceDir;
};

} // namespace blaze::lightEngine
