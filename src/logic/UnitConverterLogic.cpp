#include "red_logic_contract.hpp"

#include "logic/conversion_ratios.hpp"

void UnitConverterLogic::resetToDefaults() {}

double UnitConverterLogic::convert(const std::string& from_unit, double value,
                                   const std::string& to_unit) {
    if (from_unit == "meter" && to_unit == "feet") {
        return value * conversion_ratios::METER_TO_FEET;
    }
    if (from_unit == "meter" && to_unit == "yard") {
        return value * conversion_ratios::METER_TO_YARD;
    }
    return 0.0;
}

std::map<std::string, double> UnitConverterLogic::convertAll(const std::string& /*from_unit*/,
                                                             double /*value*/) {
    return {};
}

void UnitConverterLogic::registerUnit(const std::string& /*name*/, double /*ratio_to_meter*/) {}

void UnitConverterLogic::loadConfig(const std::string& /*path*/) {}
