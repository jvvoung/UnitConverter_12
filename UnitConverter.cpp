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

    std::string unit;
    double value = 0.0;

    std::size_t pos = input.find(':');
    if (pos == std::string::npos) {
        std::cerr << "Invalid format. Use unit:value (ex: meter:2.5)" << std::endl;
        return 1;
    }

    unit = input.substr(0, pos);
    std::string valueStr = input.substr(pos + 1);

    try {
        value = std::stod(valueStr);
    } catch (...) {
        std::cerr << "Invalid number: " << valueStr << std::endl;
        return 1;
    }

    UnitCatalog& catalog = legacyCliCatalog();
    if (!catalog.has(unit)) {
        std::cerr << "Unknown unit: " << unit << std::endl;
        return 1;
    }

    const auto converted = legacyCliConvertAll(catalog, unit, value);
    for (const auto& target_unit : legacyCliDisplayUnitOrder()) {
        std::cout << value << " " << unit << " = " << converted.at(target_unit) << " "
                  << target_unit << std::endl;
    }

    return 0;
}
