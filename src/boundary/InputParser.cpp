#include "boundary/InputParser.hpp"

#include <cctype>
#include <sstream>

namespace {

bool isValidUnitName(const std::string& unit) {
    if (unit.empty()) {
        return false;
    }
    if (!std::isalpha(static_cast<unsigned char>(unit.front()))) {
        return false;
    }
    for (std::size_t i = 1; i < unit.size(); ++i) {
        const unsigned char ch = static_cast<unsigned char>(unit[i]);
        if (!std::isalnum(ch) && unit[i] != '_') {
            return false;
        }
    }
    return true;
}

bool parsePositiveFiniteDouble(const std::string& token, double& out) {
    if (token.empty()) {
        return false;
    }
    std::size_t idx = 0;
    try {
        out = std::stod(token, &idx);
    } catch (...) {
        return false;
    }
    if (idx != token.size()) {
        return false;
    }
    return std::isfinite(out);
}

}  // namespace

ConvertParseResult InputParser::parseConvertLine(const std::string& line) {
    ConvertParseResult result;
    const auto colon = line.find(':');
    if (colon == std::string::npos) {
        result.error = ParseErrorCode::InvalidFormat;
        result.error_message = formatErrorMessage(ParseErrorCode::InvalidFormat, "");
        return result;
    }

    result.unit = line.substr(0, colon);
    result.value_token = line.substr(colon + 1);

    if (!isValidUnitName(result.unit)) {
        result.error = ParseErrorCode::EmptyUnit;
        result.error_message = formatErrorMessage(ParseErrorCode::InvalidFormat, "");
        return result;
    }

    if (!parsePositiveFiniteDouble(result.value_token, result.value)) {
        result.error = ParseErrorCode::InvalidNumber;
        result.error_message = formatErrorMessage(ParseErrorCode::InvalidNumber, result.value_token);
        return result;
    }

    result.ok = true;
    result.error = ParseErrorCode::None;
    return result;
}

std::string InputParser::formatErrorMessage(ParseErrorCode code, const std::string& token) {
    switch (code) {
        case ParseErrorCode::InvalidFormat:
            return "Invalid format. Use unit:value (ex: meter:2.5)";
        case ParseErrorCode::InvalidNumber:
            return "Invalid number: " + token;
        default:
            return "";
    }
}
