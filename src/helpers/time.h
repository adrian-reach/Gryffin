/**
 * time.h
 * ---
 * Header file for timestamp formatting functionality.
 */
#ifndef VIDEO_PLAYBACK_TIME_H
#define VIDEO_PLAYBACK_TIME_H

#include <string>
#include <cstdint>

/**
 * @brief Format a Unix timestamp into a human-readable date string.
 * @param unixTimestamp The Unix timestamp to format.
 * @return A formatted string representing the date (e.g. "Sunday, November 1, 2022")
 */
std::string FormatTimestamp(int64_t unixTimestamp);

#endif // VIDEO_PLAYBACK_TIME_H
