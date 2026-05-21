#include "DecayingUnit.hpp"

#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

// Legacy Item registry — separate from UnitConverter.cpp (extend only here in GREEN)
struct Item {
    std::string name;
    double meters_per_unit;
};

double lookupMetersPerUnit(const std::map<std::string, double>& registry,
                           const std::vector<Item>& units, const std::string& name) {
    const auto reg_it = registry.find(name);
    if (reg_it != registry.end()) {
        return reg_it->second;
    }
    for (const auto& item : units) {
        if (item.name == name) {
            return item.meters_per_unit;
        }
    }
    return 0.0;
}

}  // namespace

struct DecayingUnit::Impl {
    std::vector<Item> units{
        {"meter", 1.0},
        {"feet", 0.3048},
        {"yard", 0.9144},
    };
    std::map<std::string, double> registry;
};

DecayingUnit::DecayingUnit() : impl_(std::make_unique<Impl>()) {}

DecayingUnit::~DecayingUnit() = default;

DecayingUnit::DecayingUnit(DecayingUnit&&) noexcept = default;
DecayingUnit& DecayingUnit::operator=(DecayingUnit&&) noexcept = default;

void DecayingUnit::registerUnit(const std::string& name, double meters_per_unit) {
    if (!(meters_per_unit > 0.0)) {
        throw std::invalid_argument("meters_per_unit must be positive");
    }
    impl_->registry[name] = meters_per_unit;
}

double DecayingUnit::convert(const std::string& from_unit, double value,
                             const std::string& to_unit) const {
    const double k_from = lookupMetersPerUnit(impl_->registry, impl_->units, from_unit);
    const double k_to = lookupMetersPerUnit(impl_->registry, impl_->units, to_unit);
    return value * (k_from / k_to);
}

std::map<std::string, double> DecayingUnit::convertAll(const std::string& from_unit,
                                                       double value) const {
    std::map<std::string, double> results;
    for (const auto& item : impl_->units) {
        if (item.name == from_unit) {
            continue;
        }
        results[item.name] = convert(from_unit, value, item.name);
    }
    for (const auto& entry : impl_->registry) {
        if (entry.first == from_unit) {
            continue;
        }
        results[entry.first] = convert(from_unit, value, entry.first);
    }
    return results;
}
