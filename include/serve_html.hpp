#pragma once

#include <winsock2.h>
#include <fstream>
#include <sstream>
#include <string>
#include "create_response_body.hpp"

class Serve_HTML {
public:
	Serve_HTML() {};

    void serve_HTML_file(SOCKET client, std::string file_path) {
		std::ifstream file(file_path);
        if (!file.is_open()) {
            Http_Response_Generator::build_error_404();
            return;
        }

        std::ostringstream content;
        content << file.rdbuf();
        std::string body = content.str();

		std::string headers = Http_Response_Generator::build_success_200();

        send(client, headers.c_str(), headers.size(), 0);
        send(client, body.c_str(), body.size(), 0);
    }
};