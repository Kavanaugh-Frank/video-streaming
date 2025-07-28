#include "../include/serve_video.hpp"
#include "../include/create_response_body.hpp"
#include "../include/extract_from_request.hpp"

#include <iostream>
#include <algorithm>
#include <stdexcept>

Serve_Video::Serve_Video(SOCKET client_socket, const std::string& req, std::ifstream& video_file, std::streamsize filesize, Range& range)
    : client(client_socket), request(req), file(video_file), filesize(filesize), range(range) {
    if (!file.is_open()) {
        throw std::runtime_error("Serve_Video: File stream is not open.");
    }
    if (filesize <= 0) {
        throw std::runtime_error("Serve_Video: Invalid file size.");
    }
}

Serve_Video::~Serve_Video() {
    if (file.is_open()) file.close();
}

void Serve_Video::serve() {
    if (range.invalid) {
        std::string err_header = Http_Response_Generator::build_error_416(filesize);
        int sent = send(client, err_header.c_str(), static_cast<int>(err_header.size()), 0);
        if (sent == SOCKET_ERROR) {
            throw std::runtime_error("Serve_Video::serve - Failed to send 416 error response");
        }
        throw std::runtime_error("Serve_Video::serve - Invalid range requested (416).");
    }

    size_t content_length = range.end - range.start + 1;
    std::string success_header = Http_Response_Generator::build_success_200_or_206(range.range_detected, range, filesize);

	std::cout << "Serve_Video::serve - Sending video from " << range.start << " to " << range.end << std::endl;
    send_video(range.start, content_length, success_header);
}

void Serve_Video::send_video(size_t start, size_t length, const std::string& header) {
    int sent = send(client, header.c_str(), static_cast<int>(header.size()), 0);
    if (sent == SOCKET_ERROR) {
        throw std::runtime_error("Serve_Video::send_video - Failed to send header");
    }

    file.seekg(start);
    if (!file.good()) {
        throw std::runtime_error("Serve_Video::send_video - Failed to seek to start position");
    }

    size_t bytes_left = length;
    char buffer[8192];

    while (bytes_left > 0) {
        size_t to_read = std::min(sizeof(buffer), bytes_left);
        file.read(buffer, to_read);
        std::streamsize bytes_read = file.gcount();
        if (bytes_read <= 0) {
            throw std::runtime_error("Serve_Video::send_video - Failed to read from file");
        }
        sent = send(client, buffer, static_cast<int>(bytes_read), 0);
        if (sent == SOCKET_ERROR) {
            int err_code = WSAGetLastError();
            std::cerr << "send() failed with error code: " << err_code << std::endl;
            throw std::runtime_error("Serve_Video::send_video - Failed to send file data");
        }
        bytes_left -= bytes_read;
    }
}
