#include "serve_video.hpp"
#include "query_parameters.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <algorithm>
#include <winsock2.h>

Serve_Video::Serve_Video(SOCKET client_socket, const std::string& req, const std::string& filename)
    : client(client_socket), request(req), filename(filename) {
}

Serve_Video::~Serve_Video() {
    if (file.is_open()) file.close();
}

void Serve_Video::serve() {
    std::string full_path = "htdocs/" + filename;
    file.open(full_path, std::ios::binary | std::ios::ate);
    if (!file) {
        std::cerr << "Video file not found: " << full_path << "\n";
        send_error_404();
        return;
    }

    std::streamsize filesize = file.tellg();
    bool range_requested = false;
    Range range = extract_range_from_request(filesize, range_requested);

    if (!validate_range(range, filesize)) {
        send_error_416(filesize);
        return;
    }

    size_t content_length = range.end - range.start + 1;
    std::string header = build_success_response_header(range_requested, range, filesize);
    send_video(range.start, content_length, header);
}

Serve_Video::Range Serve_Video::extract_range_from_request(std::streamsize filesize, bool& range_detected) const {
    std::smatch match;
    Range range{ 0, static_cast<size_t>(filesize - 1) };

    if (std::regex_search(request, match, range_regex)) {
        if (match[1].matched)
            range.start = std::stoull(match[1].str());
        if (match[2].matched && !match[2].str().empty())
            range.end = std::stoull(match[2].str());
        else
            range.end = std::min(range.start + MAX_CHUNK_SIZE - 1, static_cast<size_t>(filesize - 1));

        range_detected = true;
    }

    return range;
}

bool Serve_Video::validate_range(const Range& range, std::streamsize filesize) const {
    return range.start <= range.end &&
        range.start < static_cast<size_t>(filesize) &&
        range.end < static_cast<size_t>(filesize);
}

std::string Serve_Video::build_success_response_header(bool is_partial, const Range& range, std::streamsize filesize) const {
    std::string header = is_partial ? "HTTP/1.1 206 Partial Content\r\n" : "HTTP/1.1 200 OK\r\n";
    
    if (is_partial) {
        header += "Content-Range: bytes " + std::to_string(range.start) +
            "-" + std::to_string(range.end) + "/" + std::to_string(filesize) + "\r\n";
    }

    header += "Content-Type: video/mp4\r\n";
    header += "Content-Length: " + std::to_string(range.end - range.start + 1) + "\r\n";
    header += "Accept-Ranges: bytes\r\n";
    header += "Connection: close\r\n\r\n";
    return header;
}

void Serve_Video::send_video(size_t start, size_t length, const std::string& header) {
    send(client, header.c_str(), static_cast<int>(header.size()), 0);

    file.seekg(start);
    size_t bytes_left = length;
    char buffer[8192];

    while (bytes_left > 0) {
        size_t to_read = std::min(sizeof(buffer), bytes_left);
        file.read(buffer, to_read);
        std::streamsize bytes_read = file.gcount();
        if (bytes_read <= 0) break;
        send(client, buffer, static_cast<int>(bytes_read), 0);
        bytes_left -= bytes_read;
    }
}

void Serve_Video::send_error_404() const {
    std::string err_header =
        "HTTP/1.1 404 Not Found\r\n"
        "Content-Type: text/plain\r\n"
        "Connection: close\r\n\r\n"
        "File not found.\r\n";
    send(client, err_header.c_str(), static_cast<int>(err_header.size()), 0);
}

void Serve_Video::send_error_416(std::streamsize filesize) const {
    std::string err_header =
        "HTTP/1.1 416 Range Not Satisfiable\r\n"
        "Content-Range: bytes */" + std::to_string(filesize) + "\r\n"
        "Connection: close\r\n\r\n";
    send(client, err_header.c_str(), static_cast<int>(err_header.size()), 0);
}
