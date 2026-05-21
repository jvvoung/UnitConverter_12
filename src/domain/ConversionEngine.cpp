#include "domain/ConversionEngine.hpp"

#include <cmath>

ConversionEngine::ConversionEngine(const UnitCatalog& catalog) : catalog_(catalog) {}

double ConversionEngine::convert(const std::string& from_unit, double value,
                                   const std::string& to_unit) const {
    const auto quantity = LengthQuantity::create(from_unit, value);
    return convert(quantity, to_unit);
}

double ConversionEngine::convert(const LengthQuantity& source, const std::string& to_unit) const {
    const double k_source = catalog_.metersPerUnit(source.unit());
    const double k_target = catalog_.metersPerUnit(to_unit);
    return source.value() * (k_source / k_target);
}

std::map<std::string, double> ConversionEngine::convertAll(const std::string& from_unit,
                                                           double value) const {
    std::map<std::string, double> results;
    for (const auto& name : catalog_.unitNames()) {
        if (name == from_unit) {
            continue;
        }
        results[name] = convert(from_unit, value, name);
    }
    return results;
}
