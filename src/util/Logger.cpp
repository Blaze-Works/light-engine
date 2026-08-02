#include <util/Logger.hpp>

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace blaze::lightEngine {

Logger &Logger::getInstance() noexcept {
	static Logger instance;
	return instance;
}

void Logger::setOutputFile(const std::string &filename) {
	std::lock_guard<std::mutex> lock(m_mutex);
	if (m_fileStream.is_open()) {
		m_fileStream.close();
	}
	m_fileStream.open(filename, std::ios::app);
}

void Logger::log(LogLevel level, std::string_view message) {
	std::lock_guard<std::mutex> lock(m_mutex);
	std::string logLine = formatLog(level, message);
	std::cout << logLine << std::endl;
	
	if (m_fileStream.is_open()) {
		m_fileStream << logLine << std::endl;
	}
}

Logger::~Logger() {
	if (m_fileStream.is_open()) {
		m_fileStream.close();
	}
}

std::string_view Logger::levelToString(LogLevel level) {
	switch (level) {
		case LogLevel::DEBUG: return "DEBUG";
		case LogLevel::INFO:  return "INFO";
		case LogLevel::WARN:  return "WARNING";
		case LogLevel::ERROR: return "ERROR";
		default:              return "UNKNOWN";
	}
}

std::string Logger::formatLog(LogLevel level, std::string_view message) {
	auto now = std::chrono::system_clock::now();
	auto timeTime = std::chrono::system_clock::to_time_t(now);
	auto timeTm = *std::localtime(&timeTime);
	
	std::ostringstream ss;
	ss << "[" << std::put_time(&timeTm, "%Y-%m-%d %H:%M:%S") << "] "
       << "[" << levelToString(level) << "] " << message;
	   
	return ss.str();
}

} // namespace blaze::lightEngine
