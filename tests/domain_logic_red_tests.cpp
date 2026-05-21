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

TEST_CASE("convert_feet_to_meter_returns_correct_ratio", "[logic][red]") {
    FAIL("RED");
}

TEST_CASE("convert_meter_to_yard_returns_correct_ratio", "[logic][red]") {
    FAIL("RED");
}

TEST_CASE("convert_all_returns_all_registered_targets", "[logic][red]") {
    FAIL("RED");
}

TEST_CASE("register_unit_enables_conversion", "[logic][red]") {
    FAIL("RED");
}

TEST_CASE("load_config_applies_ratios_from_file", "[logic][red]") {
    FAIL("RED");
}

TEST_CASE("load_config_invalid_path_keeps_defaults", "[logic][red]") {
    FAIL("RED");
}
