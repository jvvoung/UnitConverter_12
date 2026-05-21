#include "domain/UnitCatalog.hpp"
#include "domain/conversion_ratios.hpp"

#include <cmath>
#include <algorithm>

void UnitCatalog::add(const std::string& name, double meters_per_unit) {
    if (!(meters_per_unit > 0.0) || !std::isfinite(meters_per_unit)) {
        throw InvalidMetersPerUnitError();
    }
    units_[name] = meters_per_unit;
}

void UnitCatalog::registerUnit(const std::string& name, double meters_per_unit) {
    if (has(name)) {
        throw DuplicateUnitError(name);
    }
    add(name, meters_per_unit);
}

bool UnitCatalog::has(const std::string& name) const {
    return units_.find(name) != units_.end();
}

double UnitCatalog::metersPerUnit(const std::string& name) const {
    const auto it = units_.find(name);
    if (it == units_.end()) {
        throw UnknownUnitError(name);
    }
    return it->second;
}

std::vector<std::string> UnitCatalog::unitNames() const {
    std::vector<std::string> names;
    names.reserve(units_.size());
    for (const auto& entry : units_) {
        names.push_back(entry.first);
    }
    std::sort(names.begin(), names.end());
    return names;
}

void UnitCatalog::clear() {
    units_.clear();
}

bool UnitCatalog::empty() const {
    return units_.empty();
}

UnitCatalog bootstrap_default_three_units() {
    UnitCatalog catalog;
    catalog.add("meter", 1.0);
    catalog.add("feet", 0.3048);
    catalog.add("yard", 0.9144);
    return catalog;
}

UnitCatalog bootstrap_legacy_cli_catalog() {
    UnitCatalog catalog;
    catalog.add("meter", 1.0);
    catalog.add("feet", 1.0 / conversion_ratios::METER_TO_FEET);
    catalog.add("yard", 1.0 / conversion_ratios::METER_TO_YARD);
    return catalog;
}

DuplicateUnitError::DuplicateUnitError(const std::string& unit)
    : std::runtime_error("Unit already registered: " + unit) {}

UnknownUnitError::UnknownUnitError(const std::string& unit)
    : std::runtime_error("Unknown unit: " + unit) {}

InvalidMetersPerUnitError::InvalidMetersPerUnitError()
    : std::runtime_error("meters_per_unit must be positive and finite") {}
