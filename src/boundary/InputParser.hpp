#pragma once

#include <string>

enum class ParseErrorCode {
    None,
    InvalidFormat,
    InvalidNumber,
    EmptyUnit
};

struct ConvertParseResult {
    bool ok = false;
    ParseErrorCode error = ParseErrorCode::None;
    std::string unit;
    std::string value_token;
    double value = 0.0;
    std::string error_message;
};

class InputParser {
public:
    static ConvertParseResult parseConvertLine(const std::string& line);
    static std::string formatErrorMessage(ParseErrorCode code, const std::string& token);
};
