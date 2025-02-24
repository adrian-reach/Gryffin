#pragma once

#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <string>

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

inline std::string getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

template<typename... Args>
void log(LogLevel level, const char* format, Args&&... args) {
    const char* level_str;
    switch (level) {
        case LogLevel::DEBUG:   level_str = "DEBUG"; break;
        case LogLevel::INFO:    level_str = "INFO"; break;
        case LogLevel::WARNING: level_str = "WARN"; break;
        case LogLevel::ERROR:   level_str = "ERROR"; break;
    }
    
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), format, std::forward<Args>(args)...);
    
    std::cout << "[" << getTimestamp() << "] " << level_str << " | " << buffer << std::endl;
}

#define LOG_DEBUG(format, ...) log(LogLevel::DEBUG, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...) log(LogLevel::INFO, format, ##__VA_ARGS__)
#define LOG_WARNING(format, ...) log(LogLevel::WARNING, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) log(LogLevel::ERROR, format, ##__VA_ARGS__)