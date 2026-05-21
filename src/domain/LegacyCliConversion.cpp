#include "domain/LegacyCliConversion.hpp"

double legacyCliConvert(const UnitCatalog& catalog, const std::string& from_unit, double value,
                        const std::string& to_unit) {
    const double meters = value * catalog.metersPerUnit(from_unit);
    return meters / catalog.metersPerUnit(to_unit);
}

std::map<std::string, double> legacyCliConvertAll(const UnitCatalog& catalog,
                                                  const std::string& from_unit, double value) {
    std::map<std::string, double> results;
    for (const auto& to_unit : legacyCliDisplayUnitOrder()) {
        results[to_unit] = legacyCliConvert(catalog, from_unit, value, to_unit);
    }
    return results;
}

std::vector<std::string> legacyCliDisplayUnitOrder() {
    return {"meter", "feet", "yard"};
}
