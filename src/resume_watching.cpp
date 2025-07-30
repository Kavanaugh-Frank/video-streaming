#include "../include/resume_watching.hpp"
#include "../include/struct_definitions.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <memory>
#include <string>
#include <array>
#include <regex>

namespace fs = std::filesystem;
const std::string FILE_PATH = "last_range.txt";
const std::string VIDEO_DIR = "./htdocs/";

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


static VideoInfo parse_ffprobe_output(const std::string& output) {
    VideoInfo info;

    static const std::regex duration_regex(R"delim("duration"\s*:\s*"([0-9]+(?:\.[0-9]+)?)")delim");
    static const std::regex size_regex(R"delim("size"\s*:\s*"([0-9]+)")delim");
    static const std::regex bit_rate_regex(R"delim("bit_rate"\s*:\s*"([0-9]+)")delim");
    static const std::regex filename_regex(R"delim("filename"\s*:\s*"([^"]+)")delim");


    std::smatch match;

    if (std::regex_search(output, match, duration_regex)) {
        info.duration = std::stod(match[1].str());
    }
    if (std::regex_search(output, match, size_regex)) {
        info.size = std::stoull(match[1].str());
    }
    if (std::regex_search(output, match, bit_rate_regex)) {
        info.bit_rate = std::stoll(match[1].str());
    }
    if (std::regex_search(output, match, filename_regex)) {
        info.filename = match[1].str();
    }

    return info;
}

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

    std::string full_path = VIDEO_DIR + video_name;
    size_t file_size = get_file_size(full_path);
    if (file_size == 0) return 0;

    VideoInfo info = get_video_info(full_path);
    if (info.duration <= 0.0) return 0;

    double watched_ratio = static_cast<double>(range_end) / static_cast<double>(file_size);
    size_t last_watched_seconds = static_cast<size_t>(watched_ratio * info.duration);

    return last_watched_seconds;
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

VideoInfo Resume_Watching::get_video_info(const std::string& video_path) {
    std::cout << "Getting video info for: " << video_path << std::endl;

    std::string command = "bin\\ffprobe.exe -v quiet -print_format json -show_format \"" + video_path + "\"";

    std::array<char, 256> buffer;
    std::string result;
    FILE* pipe = _popen(command.c_str(), "r");
    if (!pipe) {
        std::cerr << "Failed to run ffprobe\n";
        return {};
    }
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }
    _pclose(pipe);

    return parse_ffprobe_output(result);
}

size_t Resume_Watching::get_video_duration(const std::string& video_path) {
    VideoInfo info = get_video_info(video_path);
    return static_cast<size_t>(info.duration);
}
