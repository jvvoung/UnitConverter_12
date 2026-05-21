#include "red_ui_contract.hpp"

#include "boundary/InputParser.hpp"

#include <stdexcept>

ConversionApplicationService::ConversionApplicationService() = default;

std::string ConversionApplicationService::convert(const std::string& input) {
    return convert(input, OutputFormat::Table);
}

std::string ConversionApplicationService::convert(const std::string& input, OutputFormat /*format*/) {
    const auto parsed = InputParser::parseConvertLine(input);
    if (!parsed.ok && parsed.error == ParseErrorCode::InvalidFormat) {
        throw std::invalid_argument(parsed.error_message);
    }
    return {};
}
