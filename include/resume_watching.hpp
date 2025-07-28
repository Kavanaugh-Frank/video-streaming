// resume_watching.hpp
#pragma once

#include <string>
#include <vector>

class Resume_Watching {
public:
    // Writes the last watched byte range for a video
    static void write_last_watched_range(const std::string& video_name, size_t start, size_t end);

    // Reads the last watched byte range for a video
    static std::pair<size_t, size_t> read_last_watched_range(const std::string& video_name);

    // Returns the last watched timestamp in seconds (approximate)
    static size_t get_last_watched_in_seconds(const std::string& video_name);

private:
    static std::vector<std::string> read_lines(const std::string& path);
    static void write_lines(const std::string& path, const std::vector<std::string>& lines);

    static std::string build_range_line(const std::string& video_name, size_t start, size_t end);
    static bool parse_range_line(const std::string& line, const std::string& video_name, size_t& range_start, size_t& range_end);

    static size_t get_file_size(const std::string& path);
    static size_t get_approx_duration(size_t total_bytes, size_t bitrate_kbps);
};
