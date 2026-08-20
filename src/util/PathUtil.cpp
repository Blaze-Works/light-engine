#include <filesystem>
#include <util/PathUtil.hpp>

#include <sstream>
#include <stdexcept>
#include <iterator>

namespace blaze::lightEngine {

fs::path PathUtil::resourceDir = "";

fs::path PathUtil::getPath(const fs::path& path, const std::vector<std::string>&) {
	return path;
}

void PathUtil::validatePath(const std::vector<std::string>&) {
	// No validation implemented
}

fs::path PathUtil::getResourceDir() {
	return PathUtil::resourceDir;
}

void PathUtil::setResourceDir(const std::string& path) {
	PathUtil::resourceDir = fs::path(path);
}

std::ifstream PathUtil::readResourceFileAsStream(const std::string& path) {
	std::ifstream file(path, std::ios::binary | std::ios::ate);
	if (!file) {
		throw std::runtime_error("File not found: " + path);
	}

	return file;
}

std::string PathUtil::readResourceFileAsString(const std::string& path) {
	std::ifstream file(path/*, std::ios::binary*/);
	if (!file) {
		throw std::runtime_error("File not found: " + path);
	}

	std::ostringstream ss;
	ss << file.rdbuf();
	return ss.str();
}

std::vector<uint8_t> PathUtil::readResourceFileAsBytes(const std::string& path) {
	std::ifstream file(path, std::ios::binary);
	if (!file) {
		throw std::runtime_error("File not found: " + path);
	}

	return std::vector<uint8_t>(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}

std::vector<std::string> PathUtil::readResourceFileAsLines(const std::string& path) {
	std::ifstream file(path);
	if (!file) {
		throw std::runtime_error("File not found: " + path);
	}

	std::vector<std::string> lines;
	std::string line;
	while (std::getline(file, line)) {
		lines.push_back(line);
	}

	return lines;
}

std::string PathUtil::join(const std::string& path1, const std::string& path2) {
	return PathUtil::join(fs::path(path1), fs::path(path2));
}

std::string PathUtil::join(const fs::path& path1, const fs::path& path2) {
	fs::path combined = fs::path(path1) / fs::path(path2);
	return combined.lexically_normal().string();
}

std::string PathUtil::resolveResource(const fs::path& path) {
	return PathUtil::join(PathUtil::resourceDir, path);
}

fs::path PathUtil::getResourcePath(const std::string& path) {
	fs::path p(path);
	if (!fs::exists(p)) {
		throw std::runtime_error("File not found: " + path);
	}

	return fs::absolute(p);
}

} // namespace blaze::lightEngine
