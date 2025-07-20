#pragma once

#include <string>
#include <fstream>
#include <regex>
#include <winsock2.h>

class Serve_Video {
public:
    Serve_Video(SOCKET client_socket, const std::string& req, const std::string& filename);
    ~Serve_Video();
    void serve();

private:
    static const size_t MAX_CHUNK_SIZE = 1024 * 1024;

    SOCKET client;
    std::string request;
    std::string filename;
    std::ifstream file;

    struct Range {
        size_t start;
        size_t end;
    };

    static inline const std::regex range_regex = std::regex("Range: bytes=(\\d*)-(\\d*)");

    Range extract_range_from_request(std::streamsize filesize, bool& range_detected) const;
    bool validate_range(const Range& range, std::streamsize filesize) const;
    std::string build_success_response_header(bool is_partial, const Range& range, std::streamsize filesize) const;
    void send_video(size_t start, size_t length, const std::string& header);
    void send_error_404() const;
    void send_error_416(std::streamsize filesize) const;
};
