#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <winsock2.h>
#include "./open_video_file.hpp"
#include "./struct_definitions.hpp"
#include "./resume_watching.hpp"

class Video_Request_Handler {
public:
	Video_Request_Handler(SOCKET& client_socket)
		: client_socket(client_socket){
	};

	~Video_Request_Handler() {
		delete video_file_ptr;
		if (client_socket != INVALID_SOCKET) {
			closesocket(client_socket);
		}
	};

	void serve_video(const std::string& request_string);

private:
	SOCKET client_socket;
	Open_Video_File* video_file_ptr = nullptr;
	std::unordered_map<std::string, std::string> query_params;
	Range extracted_range_from_request;

	void parse_video_name_from_request(const std::string& request_string);
	void open_video_file(const std::string& filename);
	void parse_range_from_request(const std::string& request_string);
	void write_last_watched_range(const std::string& filename, size_t start, size_t end);
};