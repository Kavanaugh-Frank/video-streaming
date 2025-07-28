#include "../include/resume_watching.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace fs = std::filesystem;
const std::string FILE_PATH = "last_range.txt";
const std::string VIDEO_DIR = "./htdocs/";
const size_t DEFAULT_BITRATE_KBPS = 1000;
// ---------- Utility ----------

std::string Resume_Watching::build_range_line(const std::string& video_name, size_t start, size_t end) {
    return video_name + " " + std::to_string(start) + "-" + std::to_string(end);
}

bool Resume_Watching::parse_range_line(const std::string& line, const std::string& video_name, size_t& range_start, size_t& range_end) {
    if (line.find(video_name) != 0) return false;

    size_t start_pos = line.find(' ') + 1;
    size_t dash_pos = line.find('-', start_pos);
    if (start_pos == std::string::npos || dash_pos == std::string::npos) return false;

    range_start = std::stoull(line.substr(start_pos, dash_pos - start_pos));
    range_end = std::stoull(line.substr(dash_pos + 1));
    return true;
}

// ---------- File I/O ----------

std::vector<std::string> Resume_Watching::read_lines(const std::string& path) {
    std::vector<std::string> lines;
    std::ifstream infile(path);
    std::string line;
    while (std::getline(infile, line)) {
        lines.push_back(line);
    }
    return lines;
}

void Resume_Watching::write_lines(const std::string& path, const std::vector<std::string>& lines) {
    std::ofstream outfile(path, std::ios::trunc);
    if (!outfile.is_open()) {
        throw std::runtime_error("Could not open " + path + " for writing.");
    }
    for (const auto& line : lines) {
        outfile << line << '\n';
    }
}

// ---------- Public Methods ----------

void Resume_Watching::write_last_watched_range(const std::string& video_name, size_t start, size_t end) {
    std::vector<std::string> lines = read_lines(FILE_PATH);
    bool updated = false;

    for (auto& line : lines) {
        if (line.find(video_name) == 0) {
            line = build_range_line(video_name, start, end);
            updated = true;
            break;
        }
    }

    if (!updated) {
        lines.push_back(build_range_line(video_name, start, end));
    }

    write_lines(FILE_PATH, lines);
}

std::pair<size_t, size_t> Resume_Watching::read_last_watched_range(const std::string& video_name) {
    if (!fs::exists(FILE_PATH)) return { 0, 0 };

    std::ifstream file(FILE_PATH);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open " + FILE_PATH);
    }

    std::string line;
    while (std::getline(file, line)) {
        size_t start = 0, end = 0;
        if (parse_range_line(line, video_name, start, end)) {
            return { start, end };
        }
    }
    return { 0, 0 };
}

size_t Resume_Watching::get_last_watched_in_seconds(const std::string& video_name) {
    auto [range_start, range_end] = read_last_watched_range(video_name);
    if (range_start == 0 && range_end == 0) return 0;

    size_t file_size = get_file_size(VIDEO_DIR + video_name);
    if (file_size == 0) return 0;

    size_t approx_duration = get_approx_duration(file_size, DEFAULT_BITRATE_KBPS);
    double watched_ratio = static_cast<double>(range_start) / file_size;

    return static_cast<size_t>(watched_ratio * approx_duration);
}

size_t Resume_Watching::get_file_size(const std::string& path) {
    try {
        return fs::file_size(path);
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 0;
    }
}

size_t Resume_Watching::get_approx_duration(size_t total_bytes, size_t bitrate_kbps) {
    size_t total_kbits = (total_bytes * 8) / 1024;  // 1 KB = 1024 bytes
    return total_kbits / bitrate_kbps;
}
