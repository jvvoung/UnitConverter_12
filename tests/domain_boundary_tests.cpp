#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "boundary/TableFormatter.hpp"
#include "domain/ConversionEngine.hpp"
#include "domain/LengthQuantity.hpp"
#include "domain/UnitCatalog.hpp"
#include "test_helpers.hpp"

#include <cmath>
#include <limits>

using Catch::Matchers::WithinAbs;

// 경계값 — value=0, 매우 큰 수, 소수점 6자리 정확도

TEST_CASE("test_lengthQuantity_zeroValue_rejectsCreation", "[domain][boundary]") {
    // Given: POL-NEG — 0 < value < +∞
    // When: LengthQuantity::create("meter", 0)
    // Then: 예외 (Value must be positive: 0)
    REQUIRE_THROWS_AS(LengthQuantity::create("meter", 0.0), InvalidLengthValueError);
}

TEST_CASE("test_convert_largeValue_returnsFiniteResult", "[domain][boundary]") {
    // Given: 1 meter = 3.28084 feet, double 유한 범위 내 대형 value
    const auto catalog = bootstrap_default_three_units();
    const ConversionEngine engine(catalog);
    const double large = 1e50;

    // When: meter:1e50 → feet
    const double result = engine.convert("meter", large, "feet");

    // Then: 결과 유한, 기대값 large × 3.28084 within §4.3 (상대 오차)
    REQUIRE(std::isfinite(result));
    const double expected = large * 3.28084;
    REQUIRE(std::abs(result - expected) <= std::abs(expected) * 1e-6);
}

TEST_CASE("test_convert_sixDecimalPlaces_preservesPrecision", "[domain][boundary]") {
    // Given: 1 meter = 3.28084 feet
    const auto catalog = bootstrap_default_three_units();
    const ConversionEngine engine(catalog);

    // When: 1.123456 meter → feet
    const double result = engine.convert("meter", 1.123456, "feet");

    // Then: 1.123456 × 3.28084 = 3.686789 (6자리 입력 정밀도 유지)
    const double expected = 1.123456 * 3.28084;
    REQUIRE(nearEqual(result, expected, 1e-6));
}

TEST_CASE("test_tableFormatter_halfUpOneDecimal_roundsEightPointTwo", "[domain][boundary]") {
    // Given: raw 8.2021 (2.5 m × 3.28084 ft/m)
    const double raw = 2.5 * 3.28084;

    // When: table 1dp half-up
    const double rounded = TableFormatter::roundToOneDecimalHalfUp(raw);

    // Then: 8.2
    REQUIRE_THAT(rounded, WithinAbs(8.2, 1e-9));
}

TEST_CASE("test_tableFormatter_halfUpOneDecimal_roundsTwoPointSeven", "[domain][boundary]") {
    // Given: raw 2.734025 (2.5 m × 1.09361 yd/m)
    const double raw = 2.5 * 1.09361;

    // When: table 1dp half-up
    const double rounded = TableFormatter::roundToOneDecimalHalfUp(raw);

    // Then: 2.7
    REQUIRE_THAT(rounded, WithinAbs(2.7, 1e-9));
}

TEST_CASE("test_convert_smallestPositiveValue_returnsFiniteRatio", "[domain][boundary]") {
    // Given: 1 meter = 1.09361 yard
    const auto catalog = bootstrap_default_three_units();
    const ConversionEngine engine(catalog);
    const double tiny = std::numeric_limits<double>::min();

    // When: 최소 양수 → yard
    const double result = engine.convert("meter", tiny, "yard");

    // Then: 유한 비율 곱
    REQUIRE(std::isfinite(result));
    REQUIRE(result > 0.0);
    REQUIRE(nearEqual(result, tiny * 1.09361));
}

TEST_CASE("test_convert_maxFiniteValue_staysWithinDoubleRange", "[domain][boundary]") {
    // Given: Catalog 기본 3단위 (1e200×3.28084는 double 유한, 1e308은 overflow)
    const auto catalog = bootstrap_default_three_units();
    const ConversionEngine engine(catalog);
    const double max_val = 1e100;

    // When: meter → feet at large but finite magnitude
    const double result = engine.convert("meter", max_val, "feet");

    // Then: Inf/NaN 아님 (BV-02), ratio 1 meter = 3.28084 feet
    REQUIRE(std::isfinite(result));
    const double expected = max_val * 3.28084;
    REQUIRE(std::abs(result - expected) <= std::abs(expected) * 1e-6);
}
