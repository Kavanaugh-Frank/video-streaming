#pragma once

#include <string>
#include <winsock2.h>
#include <fstream>
#include <stdexcept>

#include "./serve_html.hpp"
#include "./create_response_body.hpp"

class HTML_Request_Handler {
public:
	HTML_Request_Handler(SOCKET client_socket, std::string file_path) :
		client_socket(client_socket), file_path(std::move(file_path)) {
	};

	~HTML_Request_Handler() {
		if (client_socket != INVALID_SOCKET) {
			closesocket(client_socket);
		}
	}

	void serve_HTML() {
		try {
			check_file_exists();
		}
		catch (const std::runtime_error& e) {
			std::string error_response = Http_Response_Generator::build_error_404();
			send(client_socket, error_response.c_str(), static_cast<int>(error_response.size()), 0);
			return;
		}
		Serve_HTML serve_HTML;
		serve_HTML.serve_HTML_file(client_socket, file_path);
	}
private:
	SOCKET client_socket;
	std::string file_path;

	void check_file_exists() {
		std::ifstream file(file_path);
		if (!file.is_open()) {
			throw std::runtime_error("File not found: " + file_path);
		}
		file.close();
	};
};