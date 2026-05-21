#pragma once

#include "boundary/InputParser.hpp"

#include <string>

namespace cli_error_messages {

inline std::string formatParseError(ParseErrorCode code, const std::string& token) {
    return InputParser::formatErrorMessage(code, token);
}

inline std::string unknownUnit(const std::string& unit) {
    return "Unknown unit: " + unit;
}

}  // namespace cli_error_messages
