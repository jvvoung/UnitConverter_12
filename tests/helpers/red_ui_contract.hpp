#pragma once

// RED contract — ConversionApplicationService (no impl, Track A)
// Input: std::string "unit:value"
// Output: "2.5 meter = 8.202100 feet" or JSON
// Exception: std::invalid_argument

#include <string>

enum class OutputFormat { Table, Json };

class ConversionApplicationService {
public:
    ConversionApplicationService();

    std::string convert(const std::string& input);
    std::string convert(const std::string& input, OutputFormat format);
};
