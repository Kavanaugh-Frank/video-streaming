#pragma once
#include <cstddef>

struct Range {
    size_t start;
    size_t end;
	bool range_detected = false;
	bool invalid = false;
};