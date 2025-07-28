#pragma once

#include <string>
#include <vector>
#include <unordered_map>

class Query_Parameters {
public:
    static std::unordered_map<std::string, std::string> get_query_params(
        const std::string& request,
        const std::vector<std::string>& expected_params
    );

private:
    static std::string extract_url(const std::string& request);
    static std::string extract_query_string(const std::string& url);
    static std::unordered_map<std::string, std::string> parse_query_string(
        const std::string& query_string,
        const std::vector<std::string>& expected_params
    );
};
