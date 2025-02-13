/**
 * @file logging.h
 * @brief Logging functions for the engine
 */

#pragma once

#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>

enum class LogLevel
{
    Debug,
    Info,
    Warning,
    Error
};

inline void logMessage(LogLevel level, const std::string &message);

#if __cplusplus >= 202002L
#include <format>
template <typename... Args>
inline void logMessage(LogLevel level, const std::string &formatStr, Args &&...args)
{
    logMessage(level, std::format(formatStr, std::forward<Args>(args)...));
}
#else
#include <cstdio>
template <typename... Args>
inline void logMessage(LogLevel level, const std::string &formatStr, Args... args)
{
    char buffer[1024];
    std::snprintf(buffer, sizeof(buffer), formatStr.c_str(), args...);
    logMessage(level, std::string(buffer));
}
#endif

inline std::string currentDateTime()
{
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm tm;
#ifdef _WIN32
    localtime_s(&tm, &now_time);
#else
    localtime_r(&now_time, &tm);
#endif
    std::ostringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

inline void logMessage(LogLevel level, const std::string &message)
{
    std::string levelStr;
    switch (level)
    {
    case LogLevel::Debug:
        levelStr = "DEBUG";
        break;
    case LogLevel::Info:
        levelStr = "INFO";
        break;
    case LogLevel::Warning:
        levelStr = "WARNING";
        break;
    case LogLevel::Error:
        levelStr = "ERROR";
        break;
    default:
        levelStr = "UNKNOWN";
        break;
    }
    std::cout << "[" << currentDateTime() << "][" << levelStr << "] " << message << std::endl;
}