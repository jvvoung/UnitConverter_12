#include "domain/UnitCatalog.hpp"
#include "logic/conversion_ratios.hpp"

#include <iostream>
#include <string>

namespace {

UnitCatalog& legacyCliCatalog() {
    static UnitCatalog catalog = [] {
        UnitCatalog units;
        units.add("meter", 1.0);
        units.add("feet", 1.0 / conversion_ratios::METER_TO_FEET);
        units.add("yard", 1.0 / conversion_ratios::METER_TO_YARD);
        return units;
    }();
    return catalog;
}

double toMeters(const UnitCatalog& catalog, const std::string& unit, double value) {
    return value * catalog.metersPerUnit(unit);
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

    const double meterValue = toMeters(catalog, unit, value);
    const double inMeters = meterValue;
    const double inFeet = meterValue * conversion_ratios::METER_TO_FEET;
    const double inYards = meterValue * conversion_ratios::METER_TO_YARD;

    std::cout << value << " " << unit << " = " << inMeters << " meter" << std::endl;
    std::cout << value << " " << unit << " = " << inFeet << " feet" << std::endl;
    std::cout << value << " " << unit << " = " << inYards << " yard" << std::endl;

    return 0;
}
