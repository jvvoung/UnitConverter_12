#include "red_ui_contract.hpp"

#include "boundary/InputParser.hpp"
#include "boundary/ValueValidator.hpp"

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
    if (parsed.ok) {
        const auto validated =
            ValueValidator::validatePositive(parsed.value, parsed.value_token);
        if (!validated.ok) {
            throw std::invalid_argument(validated.error_message);
        }
    }
    return {};
}
