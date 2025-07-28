#pragma once
#include "./struct_definitions.hpp"
#include <fstream>
#include <string>
#include <winsock2.h>

class Serve_Video {
public:
    Serve_Video(SOCKET client_socket, const std::string& req, std::ifstream& video_file, std::streamsize filesize, Range& range);
    ~Serve_Video();

    void serve();

private:
    void send_video(size_t start, size_t length, const std::string& header);

    SOCKET client;
    std::string request;
    std::ifstream& file;
    std::streamsize filesize;
    Range range;
};
