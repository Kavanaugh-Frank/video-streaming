#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <iostream>
#include <string>
#include <vector>
#include <thread>

#include "include/extract_from_request.hpp"
#include "include/struct_definitions.hpp"
#include "include/create_response_body.hpp"
#include "include/open_video_file.hpp"
#include "include/serve_video.hpp"
#include "include/query_parameters.hpp"
#include "include/list_video_files.hpp"
#include "include/serve_html.hpp"
#include "include/resume_watching.hpp"
#include "include/video_request_handler.hpp"
#include "include/html_request_handler.hpp"

#pragma comment(lib, "ws2_32.lib")

void handle_client(SOCKET client_socket, const std::vector<std::string>& video_file_names) {
    char receive_buffer[2048];
    int bytes_received = recv(client_socket, receive_buffer, sizeof(receive_buffer) - 1, 0);

    receive_buffer[bytes_received] = '\0';
    std::string request_string(receive_buffer);

    if (request_string.find("GET /video") == 0) {
		Video_Request_Handler video_request_handler(client_socket);
		video_request_handler.serve_video(request_string);
        closesocket(client_socket);
    }
    else if (request_string.find("GET /all_videos") == 0) {
        std::string response_header = Http_Response_Generator::build_success_200();
        std::string response_body = Http_Response_Generator::build_response_body("video_names", video_file_names);

        send(client_socket, response_header.c_str(), static_cast<int>(response_header.size()), 0);
        send(client_socket, response_body.c_str(), static_cast<int>(response_body.size()), 0);
		closesocket(client_socket);
    }
    else if (request_string.find("GET /last_watched") == 0) {
        std::unordered_map<std::string, std::string> query_params;
        try {
            query_params = Query_Parameters::get_query_params(request_string, { "name" });
        }
        catch (const std::invalid_argument& ex) {
            std::cerr << "Invalid request: " << ex.what() << std::endl;
            closesocket(client_socket);
            return;
        }

        auto last_range = Resume_Watching::read_last_watched_range(query_params["name"]);
		std::cout << "Last watched range for " << query_params["name"] << ": " << last_range.first << "-" << last_range.second << std::endl;
        if (last_range.first == 0 && last_range.second == 0) {
            std::string error_response = Http_Response_Generator::build_error_404();
            send(client_socket, error_response.c_str(), static_cast<int>(error_response.size()), 0);
        }
        else {
            std::string response_header = Http_Response_Generator::build_success_200();
			size_t seconds_watched = Resume_Watching::get_last_watched_in_seconds(query_params["name"]);
            std::vector<std::string> vec;
            vec.push_back(std::to_string(seconds_watched));
            std::string response_body = Http_Response_Generator::build_response_body("last_time_watched", vec);
			send(client_socket, response_header.c_str(), static_cast<int>(response_header.size()), 0);
            send(client_socket, response_body.c_str(), static_cast<int>(response_body.size()), 0);
        }
        closesocket(client_socket);
    }
    else if (request_string.find("GET /") == 0) {
		HTML_Request_Handler html_request_handler(client_socket, "index.html");
		html_request_handler.serve_HTML();
        closesocket(client_socket);
    }
    else {
        std::string error_response = Http_Response_Generator::build_error_400();
		send(client_socket, error_response.c_str(), static_cast<int>(error_response.size()), 0);
        closesocket(client_socket);
    }
}

int main() {
    WSADATA wsa_data;
    WSAStartup(MAKEWORD(2, 2), &wsa_data);

    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    server_address.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket, (sockaddr*)&server_address, sizeof(server_address));
    listen(server_socket, SOMAXCONN);

    std::cout << "Serving video on http://localhost:8080\n";
    std::cout << "Press Ctrl+C to stop the server.\n";

    List_Video_Files video_files_lister;
    std::vector<std::string> video_file_names = video_files_lister.list_video_files("./htdocs");

	std::vector<std::thread> threads;
    while (true) {
        SOCKET client_socket = accept(server_socket, nullptr, nullptr);

        if(client_socket == SOCKET_ERROR) {
            std::cerr << "Error accepting client connection: " << WSAGetLastError() << std::endl;
            continue;
		}

        threads.emplace_back(handle_client, client_socket, std::ref(video_file_names));
    }

    for (auto& t : threads) {
        if (t.joinable())
            t.join();
    }

    WSACleanup();
}

