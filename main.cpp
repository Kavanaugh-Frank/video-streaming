#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <iostream>
#include <string>
#include <vector>
#include <thread>

#include "./src/serve_video.hpp"
#include "./src/query_parameters.hpp"
#include "./src/list_video_files.hpp"
#include "./src/create_html.hpp"

#pragma comment(lib, "ws2_32.lib")

void handle_client(SOCKET client_socket, const std::vector<std::string>& video_file_names) {

	std::cout << "Client connected.\n";

    char receive_buffer[2048];
    int bytes_received = recv(client_socket, receive_buffer, sizeof(receive_buffer) - 1, 0);

    receive_buffer[bytes_received] = '\0';
    std::string request_string(receive_buffer);
    std::cout << request_string << std::endl;

    if (request_string.find("GET /video") == 0) {
        auto query_params = Query_Parameters::get_query_params(request_string, { "name" });
        Serve_Video video_handler(client_socket, request_string, query_params["name"]);
        video_handler.serve();
        closesocket(client_socket);
    }
    else if (request_string.find("GET /") == 0) {
        std::cout << "HERE.\n";
        Create_Html html_generator;
        std::string html_content = html_generator.load_html_string(video_file_names);
        send(client_socket, html_content.c_str(), static_cast<int>(html_content.size()), 0);
        closesocket(client_socket);
    }
    else {
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

