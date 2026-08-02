#pragma once

#include <fstream>
#include <mutex>
#include <string>
#include <string_view>

#define LOG_DEBUG(msg) blaze::lightEngine::Logger::getInstance().log(blaze::lightEngine::LogLevel::DEBUG, msg)
#define LOG_INFO(msg)  blaze::lightEngine::Logger::getInstance().log(blaze::lightEngine::LogLevel::INFO,  msg);
#define LOG_WARN(msg)  blaze::lightEngine::Logger::getInstance().log(blaze::lightEngine::LogLevel::WARN,  msg);
#define LOG_ERROR(msg) blaze::lightEngine::Logger::getInstance().log(blaze::lightEngine::LogLevel::ERROR, msg);

namespace blaze::lightEngine {

enum class LogLevel {
    DEBUG,
    INFO,
    WARN,
    ERROR
};

class Logger {
public:
    static Logger& getInstance() noexcept;
    Logger(const Logger&) = delete;
    Logger& operator = (const Logger&) = delete;
    void setOutputFile(const std::string& filename);
    void log(LogLevel level, std::string_view message);

private:
    Logger() = default;
    ~Logger();
    std::ofstream m_fileStream;
    std::mutex m_mutex;
    std::string_view levelToString(LogLevel level);
    std::string formatLog(LogLevel level, std::string_view message);

    static Logger* instance;
};


} // namespace blaze::lightEngine
