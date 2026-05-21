#include "DecayingUnit.hpp"

#include <map>
#include <string>
#include <vector>

namespace {

// Legacy Item registry — separate from UnitConverter.cpp (extend only here in GREEN)
struct Item {
    std::string name;
    double meters_per_unit;
};

}  // namespace

struct DecayingUnit::Impl {
    std::vector<Item> units{
        {"meter", 1.0},
        {"feet", 0.3048},
        {"yard", 0.9144},
    };
};

DecayingUnit::DecayingUnit() : impl_(std::make_unique<Impl>()) {}

DecayingUnit::~DecayingUnit() = default;

DecayingUnit::DecayingUnit(DecayingUnit&&) noexcept = default;
DecayingUnit& DecayingUnit::operator=(DecayingUnit&&) noexcept = default;

void DecayingUnit::registerUnit(const std::string& name, double meters_per_unit) {
    (void)name;
    (void)meters_per_unit;
    // RED: 동적 등록 미구현
}

double DecayingUnit::convert(const std::string& from_unit, double value,
                             const std::string& to_unit) const {
    (void)from_unit;
    (void)value;
    (void)to_unit;
    return 0.0;  // RED: meter 허브 변환 미구현
}

std::map<std::string, double> DecayingUnit::convertAll(const std::string& from_unit,
                                                       double value) const {
    (void)from_unit;
    (void)value;
    return {};  // RED: 전 단위 변환 미구현
}
