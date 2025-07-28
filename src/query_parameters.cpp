#include "../include/query_parameters.hpp"
#include <unordered_set>
#include <unordered_map>
#include <stdexcept>
#include <sstream>

std::unordered_map<std::string, std::string> Query_Parameters::get_query_params(
    const std::string& request,
    const std::vector<std::string>& expected_params
) {
    std::string url = extract_url(request);
    if (url.empty()) {
        throw std::invalid_argument("Malformed HTTP request: URL not found.");
    }

    std::string query_string = extract_query_string(url);
    if (query_string.empty()) {
        throw std::invalid_argument("No query string found in URL.");
    }

    return parse_query_string(query_string, expected_params);
}

std::string Query_Parameters::extract_url(const std::string& request) {
    size_t start = request.find(' ');
    if (start == std::string::npos) return "";

    size_t end = request.find(' ', start + 1);
    if (end == std::string::npos) return "";

    return request.substr(start + 1, end - start - 1);
}

std::string Query_Parameters::extract_query_string(const std::string& url) {
    size_t question_mark = url.find('?');
    if (question_mark == std::string::npos) return "";
    return url.substr(question_mark + 1);
}

std::unordered_map<std::string, std::string> Query_Parameters::parse_query_string(
    const std::string& query_string,
    const std::vector<std::string>& expected_params
) {
    std::unordered_map<std::string, std::string> result;
    std::unordered_set<std::string> valid_keys(expected_params.begin(), expected_params.end());

    size_t current = 0;
    while (current < query_string.size()) {
        size_t equal_pos = query_string.find('=', current);
        if (equal_pos == std::string::npos) {
            throw std::invalid_argument("Invalid query string: missing '=' in parameter.");
        }

        std::string key = query_string.substr(current, equal_pos - current);
        size_t ampersand_pos = query_string.find('&', equal_pos + 1);

        std::string value;
        if (ampersand_pos == std::string::npos) {
            value = query_string.substr(equal_pos + 1);
            current = query_string.size();
        }
        else {
            value = query_string.substr(equal_pos + 1, ampersand_pos - equal_pos - 1);
            current = ampersand_pos + 1;
        }

        if (key.empty()) {
            throw std::invalid_argument("Query string contains an empty key.");
        }

        if (valid_keys.count(key)) {
            result[key] = value;
        }
    }

    return result;
}
