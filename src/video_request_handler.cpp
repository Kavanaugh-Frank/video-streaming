#include "../include/video_request_handler.hpp"
#include "../include/query_parameters.hpp"
#include "../include/serve_video.hpp"           
#include "../include/extract_from_request.hpp"
#include "../include/create_response_body.hpp"
#include <stdexcept>


void Video_Request_Handler::serve_video(const std::string& request_string) {
    try {
        parse_video_name_from_request(request_string);
        open_video_file(query_params["name"]);
        parse_range_from_request(request_string);

        Serve_Video handler(client_socket, request_string, video_file_ptr->get_stream(), video_file_ptr->get_size(), extracted_range_from_request);
        handler.serve();

        if (extracted_range_from_request.range_detected) {
            write_last_watched_range(query_params["name"], extracted_range_from_request.start, extracted_range_from_request.end);
        }
    }
    catch (const std::exception& ex) {
        std::string error_response = Http_Response_Generator::build_error_500(ex.what());
        send(client_socket, error_response.c_str(), static_cast<int>(error_response.size()), 0);
        return;
    }

    delete video_file_ptr;
    video_file_ptr = nullptr;
}

void Video_Request_Handler::parse_video_name_from_request(const std::string& request_string) {
    try {
        query_params = Query_Parameters::get_query_params(request_string, { "name" });
    }
    catch (const std::invalid_argument& ex) {
        throw std::invalid_argument(std::string("Could not parse query: ") + ex.what());
    }
}

void Video_Request_Handler::open_video_file(const std::string& filename) {
    try {
        video_file_ptr = new Open_Video_File(filename);
    }
    catch (const std::exception& ex) {
        throw std::invalid_argument(std::string("Error opening video file: ") + ex.what());
    }
}

void Video_Request_Handler::parse_range_from_request(const std::string& request_string) {
    Extract_From_Request extractor;
    extracted_range_from_request = extractor.extract_range_from_request(request_string, video_file_ptr->get_size());
    if (extracted_range_from_request.invalid) {
        throw std::invalid_argument("Invalid range requested.");
    }
}

void Video_Request_Handler::write_last_watched_range(const std::string& filename, size_t start, size_t end) {
    Resume_Watching::write_last_watched_range(filename, start, end);
}
