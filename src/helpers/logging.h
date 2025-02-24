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

// Helper function to convert any type to string using stringstream
template<typename T>
inline std::string toString(const T& value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}

// Specialization for strings to avoid unnecessary conversion
template<>
inline std::string toString<std::string>(const std::string& value) {
    return value;
}

template<>
inline std::string toString<const char*>(const char* const& value) {
    return value;
}

// Base case for template recursion
inline void formatString(std::string& result, size_t& argIndex, const std::string& format) {
    size_t pos = 0;
    while ((pos = result.find("{}", pos)) != std::string::npos) {
        result.replace(pos, 2, "{}");  // Keep remaining placeholders
        pos += 2;
    }
}

// Recursive template to handle multiple arguments
template<typename T, typename... Args>
inline void formatString(std::string& result, size_t& argIndex, const std::string& format, T&& value, Args&&... args) {
    size_t pos = result.find("{}");
    if (pos != std::string::npos) {
        result.replace(pos, 2, toString(std::forward<T>(value)));
        formatString(result, ++argIndex, format, std::forward<Args>(args)...);
    }
}

template<typename... Args>
inline void log(LogLevel level, const std::string& format, Args&&... args) {
    const char* level_str;
    switch (level) {
        case LogLevel::DEBUG:   level_str = "DEBUG"; break;
        case LogLevel::INFO:    level_str = "INFO"; break;
        case LogLevel::WARNING: level_str = "WARN"; break;
        case LogLevel::ERROR:   level_str = "ERROR"; break;
    }
    
    std::string message = format;
    size_t argIndex = 0;
    formatString(message, argIndex, format, std::forward<Args>(args)...);
    
    std::cout << "[" << getTimestamp() << "] " << level_str << " | " << message << std::endl;
}

#define LOG_DEBUG(format, ...) log(LogLevel::DEBUG, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...) log(LogLevel::INFO, format, ##__VA_ARGS__)
#define LOG_WARNING(format, ...) log(LogLevel::WARNING, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) log(LogLevel::ERROR, format, ##__VA_ARGS__)