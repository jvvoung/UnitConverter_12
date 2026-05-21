#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "domain/ConversionEngine.hpp"
#include "domain/UnitCatalog.hpp"
#include "test_helpers.hpp"

using Catch::Matchers::WithinAbs;

// 정상 변환 — meter→feet, meter→yard, feet→meter (역변환) 등

TEST_CASE("test_meter_to_feet_positiveValue_returnsFeetProduct", "[domain][normal]") {
    // Given: 기본 Catalog (1 meter = 3.28084 feet, k_feet = 0.3048)
    const auto catalog = bootstrap_default_three_units();
    const ConversionEngine engine(catalog);

    // When: 2.5 meter → feet (ratio: 1 meter = 3.28084 feet)
    const double result = engine.convert("meter", 2.5, "feet");

    // Then: 2.5 × 3.28084 = 8.2021
    REQUIRE_THAT(result, WithinAbs(8.2021, 1e-4));
}

TEST_CASE("test_meter_to_yard_positiveValue_returnsYardProduct", "[domain][normal]") {
    // Given: 1 meter = 1.09361 yard
    const auto catalog = bootstrap_default_three_units();
    const ConversionEngine engine(catalog);

    // When: 2.5 meter → yard
    const double result = engine.convert("meter", 2.5, "yard");

    // Then: 2.5 × 1.09361 = 2.734025
    REQUIRE_THAT(result, WithinAbs(2.734025, 1e-4));
}

TEST_CASE("test_feet_to_meter_reverseConversion_returnsMeterQuotient", "[domain][normal]") {
    // Given: 1 feet = 0.3048 meter (역변환: feet → meter)
    const auto catalog = bootstrap_default_three_units();
    const ConversionEngine engine(catalog);

    // When: 3.28084 feet → meter
    const double result = engine.convert("feet", 3.28084, "meter");

    // Then: 3.28084 × (0.3048 / 1.0) ≈ 1.0 meter
    REQUIRE_THAT(result, WithinAbs(1.0, 1e-4));
}

TEST_CASE("test_meter_to_feet_unitValue_returnsOneToOneRatio", "[domain][normal]") {
    // Given: ratio 1 meter = 3.28084 feet
    const auto catalog = bootstrap_default_three_units();
    const ConversionEngine engine(catalog);

    // When: 1.0 meter → feet
    const double result = engine.convert("meter", 1.0, "feet");

    // Then: 1.0 × 3.28084 = 3.28084
    REQUIRE_THAT(result, WithinAbs(3.28084, 1e-5));
}

TEST_CASE("test_feet_to_yard_hubPath_matchesMeterIntermediate", "[domain][normal]") {
    // Given: feet↔yard는 meter 허브 경유만 허용 (NG-03)
    const auto catalog = bootstrap_default_three_units();
    const ConversionEngine engine(catalog);

    // When: 10 feet → yard (via meter hub)
    const double direct = engine.convert("feet", 10.0, "yard");
    const double via_meter =
        engine.convert("meter", engine.convert("feet", 10.0, "meter"), "yard");

    // Then: 직접 환산 = meter 경유 환산
    REQUIRE_THAT(direct, WithinAbs(via_meter, 1e-9));
}

TEST_CASE("test_convertAll_meterOne_excludesSourceUnit", "[domain][normal]") {
    // Given: 3단위 Catalog
    const auto catalog = bootstrap_default_three_units();
    const ConversionEngine engine(catalog);

    // When: convertAll from meter:1.0 (1 meter = 3.28084 feet, 1.09361 yard)
    const auto all = engine.convertAll("meter", 1.0);

    // Then: source 제외 2건, feet·yard 포함
    REQUIRE(all.size() == 2);
    REQUIRE(all.count("feet") == 1);
    REQUIRE(all.count("yard") == 1);
    REQUIRE_THAT(all.at("feet"), WithinAbs(3.28084, 1e-5));
    REQUIRE_THAT(all.at("yard"), WithinAbs(1.09361, 1e-5));
}
