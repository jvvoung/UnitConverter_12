#include "boundary/CliErrorMessages.hpp"
#include "boundary/CliOutputFormatter.hpp"
#include "boundary/InputParser.hpp"
#include "domain/LegacyCliConversion.hpp"
#include "domain/UnitCatalog.hpp"

#include <iostream>
#include <string>

namespace {

UnitCatalog& legacyCliCatalog() {
    static UnitCatalog catalog = bootstrap_legacy_cli_catalog();
    return catalog;
}

}  // namespace

int main() {
    std::cout << "Insert value for converting (ex: meter:2.5): ";

    std::string input;
    std::getline(std::cin, input);

    const auto parsed = InputParser::parseConvertLine(input);
    if (!parsed.ok) {
        std::cerr << cli_error_messages::formatParseError(parsed.error, parsed.value_token)
                  << std::endl;
        return 1;
    }

    UnitCatalog& catalog = legacyCliCatalog();
    if (!catalog.has(parsed.unit)) {
        std::cerr << cli_error_messages::unknownUnit(parsed.unit) << std::endl;
        return 1;
    }

    const auto converted = legacyCliConvertAll(catalog, parsed.unit, parsed.value);
    for (const auto& target_unit : legacyCliDisplayUnitOrder()) {
        std::cout << CliOutputFormatter::formatLine(parsed.value_token, parsed.unit,
                                                    converted.at(target_unit), target_unit)
                  << std::endl;
    }

    return 0;
}
