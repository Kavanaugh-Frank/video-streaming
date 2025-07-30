#pragma once
#include <cstddef>

struct Range {
    size_t start;
    size_t end;
	bool range_detected = false;
	bool invalid = false;
};

struct VideoInfo {
    double duration = 0.0;
    size_t size = 0;
    long long int bit_rate = 0;
    std::string filename;
};