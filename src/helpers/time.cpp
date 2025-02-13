/**
 * @file time.cpp
 * Helper function related to time.
 */
#include "time.h"
#include <iomanip>
#include <sstream>
#include <ctime>

std::string FormatTimestamp(int64_t unixTimestamp)
{
    // Convert the timestamp to a time_t type
    std::time_t time = static_cast<std::time_t>(unixTimestamp);

    // Convert to a tm struct for local time
    std::tm tmStruct;
#ifdef _WIN32
    localtime_s(&tmStruct, &time); // Windows
#else
    localtime_r(&time, &tmStruct); // Linux/macOS
#endif

    // Format the time into a human-readable string
    std::ostringstream oss;
    oss << std::put_time(&tmStruct, "%A, %B %d, %Y"); // Example: "Sunday, November 1, 2022"

    return oss.str();
}
