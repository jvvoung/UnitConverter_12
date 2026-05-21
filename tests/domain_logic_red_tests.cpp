#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "red_logic_contract.hpp"

using Catch::Matchers::WithinAbs;

// Track B - Domain / Logic (UnitConverterLogic)

TEST_CASE("convert_meter_to_feet_returns_correct_ratio", "[logic][green]") {
    UnitConverterLogic::resetToDefaults();
    const double result = UnitConverterLogic::convert("meter", 2.5, "feet");
    REQUIRE_THAT(result, WithinAbs(8.20210, 1e-5));
}

TEST_CASE("convert_feet_to_meter_returns_correct_ratio", "[logic][green]") {
    UnitConverterLogic::resetToDefaults();
    const double result = UnitConverterLogic::convert("feet", 1.0, "meter");
    REQUIRE_THAT(result, WithinAbs(0.30480, 1e-5));
}

TEST_CASE("convert_meter_to_yard_returns_correct_ratio", "[logic][green]") {
    UnitConverterLogic::resetToDefaults();
    const double result = UnitConverterLogic::convert("meter", 1.0, "yard");
    REQUIRE_THAT(result, WithinAbs(1.09361, 1e-5));
}

TEST_CASE("convert_all_returns_all_registered_targets", "[logic][green]") {
    UnitConverterLogic::resetToDefaults();
    const auto all = UnitConverterLogic::convertAll("meter", 2.5);
    REQUIRE(all.size() == 2);
    REQUIRE(all.count("feet") == 1);
    REQUIRE(all.count("yard") == 1);
    REQUIRE_THAT(all.at("feet"), WithinAbs(8.20210, 1e-5));
    REQUIRE_THAT(all.at("yard"), WithinAbs(2.734025, 1e-5));
}

TEST_CASE("register_unit_enables_conversion", "[logic][green]") {
    UnitConverterLogic::resetToDefaults();
    UnitConverterLogic::registerUnit("cubit", 0.4572);
    const double result = UnitConverterLogic::convert("cubit", 2.0, "meter");
    REQUIRE_THAT(result, WithinAbs(0.9144, 1e-5));
}

TEST_CASE("load_config_applies_ratios_from_file", "[logic][red]") {
    FAIL("RED");
}

TEST_CASE("load_config_invalid_path_keeps_defaults", "[logic][red]") {
    FAIL("RED");
}
