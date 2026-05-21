#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "red_logic_contract.hpp"
#include "test_helpers.hpp"

#include <cmath>

using Catch::Matchers::WithinAbs;

// Track B — Domain / Logic RED
// Current: UnitConverterLogic unimplemented -> link FAIL (RED)

TEST_CASE("TC-B-01: convert meter to feet within 1e-5", "[logic][red]") {
    // Scenario: meter -> feet (2.5 * 3.28084 = 8.2021)
    UnitConverterLogic::resetToDefaults();

    const double result = UnitConverterLogic::convert("meter", 2.5, "feet");

    // Invariant: 1 meter = 3.28084 feet, |result - expected| <= 1e-5
    REQUIRE_THAT(result, WithinAbs(8.20210, 1e-5));
}

TEST_CASE("TC-B-01b: convert feet to meter reverse within 1e-5", "[logic][red]") {
    // Scenario: feet -> meter reverse (3.28084 feet ~= 1 meter)
    UnitConverterLogic::resetToDefaults();

    const double result = UnitConverterLogic::convert("feet", 3.28084, "meter");

    // Invariant: meter hub consistency, reverse error <= 1e-5
    REQUIRE_THAT(result, WithinAbs(1.0, 1e-5));
}

TEST_CASE("TC-B-02: convert meter to yard within 1e-5", "[logic][red]") {
    // Scenario: meter -> yard (1.0 * 1.09361 = 1.09361)
    UnitConverterLogic::resetToDefaults();

    const double result = UnitConverterLogic::convert("meter", 1.0, "yard");

    // Invariant: 1 meter = 1.09361 yard, |result - 1.09361| <= 1e-5
    REQUIRE_THAT(result, WithinAbs(1.09361, 1e-5));
}

TEST_CASE("TC-B-03: convertAll returns all registered target units", "[logic][red]") {
    // Scenario: meter:2.5 -> feet + yard
    UnitConverterLogic::resetToDefaults();

    const auto all = UnitConverterLogic::convertAll("meter", 2.5);

    // Invariant: all targets except source, finite positive values
    REQUIRE(all.size() == 2);
    REQUIRE(all.count("feet") == 1);
    REQUIRE(all.count("yard") == 1);
    REQUIRE(std::isfinite(all.at("feet")));
    REQUIRE(std::isfinite(all.at("yard")));
    REQUIRE_THAT(all.at("feet"), WithinAbs(8.20210, 1e-5));
    REQUIRE_THAT(all.at("yard"), WithinAbs(2.734025, 1e-5));
}

TEST_CASE("TC-B-04: registerUnit enables conversion for new unit", "[logic][red]") {
    // Scenario: register cubit (0.4572 m) then cubit -> meter
    UnitConverterLogic::resetToDefaults();
    UnitConverterLogic::registerUnit("cubit", 0.4572);

    const double result = UnitConverterLogic::convert("cubit", 2.0, "meter");

    // Invariant: post-register conversion works, 2 * 0.4572 = 0.9144
    REQUIRE_THAT(result, WithinAbs(0.9144, 1e-5));
}

TEST_CASE("TC-B-05: loadConfig applies ratios from valid file", "[logic][red]") {
    // Scenario: load units_valid.json then meter -> feet
    UnitConverterLogic::resetToDefaults();
    UnitConverterLogic::loadConfig(testDataPath("units_valid.json"));

    const double result = UnitConverterLogic::convert("meter", 2.5, "feet");

    // Invariant: config ratios applied, 2.5 * 3.28084 within 1e-5
    REQUIRE_THAT(result, WithinAbs(8.20210, 1e-5));
}

TEST_CASE("TC-B-06: loadConfig invalid path keeps default ratios", "[logic][red]") {
    // Scenario: missing file -> defaults (3.28084/1.09361), no throw
    UnitConverterLogic::resetToDefaults();

    REQUIRE_NOTHROW(UnitConverterLogic::loadConfig(testDataPath("no_such_file.json")));

    const double feet = UnitConverterLogic::convert("meter", 1.0, "feet");
    const double yard = UnitConverterLogic::convert("meter", 1.0, "yard");

    // Invariant: default catalog unchanged
    REQUIRE_THAT(feet, WithinAbs(3.28084, 1e-5));
    REQUIRE_THAT(yard, WithinAbs(1.09361, 1e-5));
}
