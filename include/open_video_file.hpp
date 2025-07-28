#pragma once
#include <fstream>
#include <string>
#include <stdexcept>

class Open_Video_File {
private:
    std::ifstream file_stream;
    std::streamsize file_size;

public:
    Open_Video_File(const std::string& filename) {
        std::string path = "htdocs/" + filename;
        file_stream.open(path, std::ios::binary | std::ios::ate);
        if (!file_stream.is_open()) {
            throw std::runtime_error("Failed to open video file: " + path);
        }

        file_size = file_stream.tellg();
        file_stream.seekg(0);
    }

    std::ifstream& get_stream() {
        return file_stream;
    }

    std::streamsize get_size() const {
        return file_size;
    }

    bool is_open() const {
        return file_stream.is_open();
    }
};
