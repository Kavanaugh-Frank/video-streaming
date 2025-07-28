#pragma once

#include <string>
#include <vector>
#include <iostream>
#include "./struct_definitions.hpp"

class Http_Response_Generator {
public:
	Http_Response_Generator(){}

	static std::string build_success_200_or_206(bool range_requested, const Range& range, std::streamsize filesize) {
		std::string header = range_requested ? "HTTP/1.1 206 Partial Content\r\n" : "HTTP/1.1 200 OK\r\n";

		if (range_requested) {
			header += "Content-Range: bytes " + std::to_string(range.start) +
				"-" + std::to_string(range.end) + "/" + std::to_string(filesize) + "\r\n";
		}

		header += "Content-Type: video/mp4\r\n";
		header += "Content-Length: " + std::to_string(range.end - range.start + 1) + "\r\n";
		header += "Accept-Ranges: bytes\r\n";
		header += "Connection: close\r\n\r\n";
		return header;
	}

	static std::string build_error_404() {
		std::string err_header =
			"HTTP/1.1 404 Not Found\r\n"
			"Content-Type: text/plain\r\n"
			"Connection: close\r\n\r\n"
			"File not found.\r\n";
		return err_header;
	}

	static std::string build_success_200() {
		std::string success_header =
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/html\r\n"
			"Connection: close\r\n\r\n";
		return success_header;
	}

	static std::string build_error_416(std::streamsize filesize) {
		std::string err_header =
			"HTTP/1.1 416 Range Not Satisfiable\r\n"
			"Content-Type: text/plain\r\n"
			"Content-Range: bytes */" + std::to_string(filesize) + "\r\n"
			"Connection: close\r\n\r\n"
			"Requested range not satisfiable.\r\n";
		return err_header;
	}

	static std::string build_error_400() {
		std::string err_header =
			"HTTP/1.1 400 Bad Request\r\n"
			"Content-Type: text/plain\r\n"
			"Connection: close\r\n\r\n"
			"Bad request.\r\n";
		return err_header;
	}

	static std::string build_error_500(const std::string& ex_msg) {
		std::string err_header =
			"HTTP/1.1 500 Internal Server Error\r\n"
			"Content-Type: text/plain\r\n"
			"Connection: close\r\n\r\n"
			"Internal server error:\r\n" + ex_msg + "\r\n";
		return err_header;
	}

	static std::string build_response_body(const std::string key, const std::vector<std::string>& key_values) {
		std::string response_body = "{ \"" + key + "\": [";
		for (size_t i = 0; i < key_values.size(); ++i) {
			response_body += "\"" + key_values[i] + "\"";
			if (i < key_values.size() - 1) {
				response_body += ",";
			}
		}
		response_body += "] }";
		std::cout << "Response body: " << response_body << std::endl;
		return response_body;
	}
};