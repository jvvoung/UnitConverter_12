#include "red_ui_contract.hpp"

#include "boundary/InputParser.hpp"
#include "boundary/ValueValidator.hpp"
#include "domain/ConversionEngine.hpp"
#include "domain/UnitCatalog.hpp"

#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace {

UnitCatalog& serviceCatalog() {
    static UnitCatalog catalog = bootstrap_default_three_units();
    return catalog;
}

std::string formatFeetConversionLine(const std::string& value_token, const std::string& unit,
                                     double feet_value) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6) << feet_value;
    return value_token + " " + unit + " = " + oss.str() + " feet";
}

}  // namespace

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
        if (!serviceCatalog().has(parsed.unit)) {
            throw std::invalid_argument("Unknown unit: " + parsed.unit);
        }
        const ConversionEngine engine(serviceCatalog());
        const double feet_value = engine.convert(parsed.unit, parsed.value, "feet");
        return formatFeetConversionLine(parsed.value_token, parsed.unit, feet_value);
    }
    return {};
}
