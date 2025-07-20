#include "query_parameters.hpp"
#include <unordered_set>
#include <unordered_map>

std::unordered_map<std::string, std::string> Query_Parameters::get_query_params(
    const std::string& request,
    const std::vector<std::string>& params
) {
    std::string url = extract_url(request);
    if (url.empty()) return {};

    std::string query_string = get_query_string(url);
    if (query_string.empty()) return {};

    return parse_query_string(query_string, params);
}

std::string Query_Parameters::extract_url(const std::string& request) {
    size_t start = request.find(' ');
    if (start == std::string::npos) return "";
    start++;

    size_t end = request.find(' ', start);
    if (end == std::string::npos) return "";

    return request.substr(start, end - start);
}

std::string Query_Parameters::get_query_string(const std::string& url) {
    size_t qpos = url.find('?');
    if (qpos == std::string::npos) return "";
    return url.substr(qpos + 1);
}

std::unordered_map<std::string, std::string> Query_Parameters::parse_query_string(
    const std::string& query_string,
    const std::vector<std::string>& params
) {
    std::unordered_map<std::string, std::string> results;
    std::unordered_set<std::string> param_set(params.begin(), params.end());

    size_t pos = 0;
    while (pos < query_string.size()) {
        size_t eq_pos = query_string.find('=', pos);
        if (eq_pos == std::string::npos) break;

        std::string name = query_string.substr(pos, eq_pos - pos);

        size_t amp_pos = query_string.find('&', eq_pos + 1);
        if (amp_pos == std::string::npos) amp_pos = query_string.size();

        std::string value = query_string.substr(eq_pos + 1, amp_pos - eq_pos - 1);

        if (param_set.count(name)) {
            results[name] = value;
        }

        pos = amp_pos + 1;
    }

    return results;
}
