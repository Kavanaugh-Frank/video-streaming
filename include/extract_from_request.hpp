#pragma once

#include <regex>
#include <string>
#include "./struct_definitions.hpp"

class Extract_From_Request {
public:
    Extract_From_Request() {}

    Range extract_range_from_request(const std::string& request, std::streamsize filesize) {
        std::smatch match;
        Range range{ 0, static_cast<size_t>(filesize - 1), false, false };

        if (std::regex_search(request, match, range_regex)) {
            if (match[1].matched)
                range.start = std::stoull(match[1].str());
            if (match[2].matched && !match[2].str().empty())
                range.end = std::stoull(match[2].str());
            else
                range.end = std::min(range.start + MAX_CHUNK_SIZE - 1, static_cast<size_t>(filesize - 1));

            range.range_detected = true;
        }
		range.invalid = !is_valid_range(range, filesize);
        return range;
    }
private:
    static const size_t MAX_CHUNK_SIZE = 1024 * 1024;
    static inline const std::regex range_regex = std::regex("Range: bytes=(\\d*)-(\\d*)");

    bool is_valid_range(const Range& range, std::streamsize filesize) const {
        return range.start <= range.end &&
               range.start < static_cast<size_t>(filesize) &&
               range.end < static_cast<size_t>(filesize);
	}
};