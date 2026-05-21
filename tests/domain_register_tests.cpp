#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "domain/ConversionEngine.hpp"
#include "domain/UnitCatalog.hpp"
#include "test_helpers.hpp"

using Catch::Matchers::WithinAbs;

// 동적 등록 — registerUnit 후 변환

TEST_CASE("test_registerUnit_cubitAfterRegister_convertsToMeter", "[domain][register]") {
    // Given: REGISTER 1 cubit = 0.4572 meter
    UnitCatalog catalog = bootstrap_default_three_units();
    catalog.registerUnit("cubit", 0.4572);
    const ConversionEngine engine(catalog);

    // When: cubit:2 → meter (2 × 0.4572)
    const double result = engine.convert("cubit", 2.0, "meter");

    // Then: ≈ 0.9144 meter
    REQUIRE_THAT(result, WithinAbs(0.9144, 1e-4));
}

TEST_CASE("test_registerUnit_cubitToFeet_usesMeterHub", "[domain][register]") {
    // Given: cubit k=0.4572, 1 meter = 3.28084 feet
    UnitCatalog catalog = bootstrap_default_three_units();
    catalog.registerUnit("cubit", 0.4572);
    const ConversionEngine engine(catalog);

    // When: 1 cubit → feet
    const double result = engine.convert("cubit", 1.0, "feet");

    // Then: 0.4572 / 0.3048 ≈ 1.5 feet (meter 허브)
    const double expected = 0.4572 / 0.3048;
    REQUIRE_THAT(result, WithinAbs(expected, 1e-4));
}

TEST_CASE("test_registerUnit_duplicateName_throwsDuplicateUnit", "[domain][register]") {
    // Given: feet 이미 등록됨
    UnitCatalog catalog = bootstrap_default_three_units();

    // When/Then: DUPLICATE_UNIT
    REQUIRE_THROWS_AS(catalog.registerUnit("feet", 0.3048), DuplicateUnitError);
}

TEST_CASE("test_registerUnit_zeroK_throwsInvalidMetersPerUnit", "[domain][register]") {
    // Given: k > 0 finite
    UnitCatalog catalog;

    // When/Then: k=0 거부
    REQUIRE_THROWS_AS(catalog.registerUnit("link", 0.0), InvalidMetersPerUnitError);
}

TEST_CASE("test_registerUnit_negativeK_throwsInvalidMetersPerUnit", "[domain][register]") {
    // Given: k must be positive
    UnitCatalog catalog;

    // When/Then: k=-1 거부
    REQUIRE_THROWS_AS(catalog.registerUnit("fathom", -1.0), InvalidMetersPerUnitError);
}

TEST_CASE("test_registerUnit_preservesDefaultGoldenTrio", "[domain][register]") {
    // Given: cubit 추가 후에도 meter:2.5 golden 불변 (RR-04)
    UnitCatalog catalog = bootstrap_default_three_units();
    catalog.registerUnit("cubit", 0.4572);
    const ConversionEngine engine(catalog);

    // When: meter:2.5 → feet (1 meter = 3.28084 feet)
    const double feet = engine.convert("meter", 2.5, "feet");

    // Then: 8.2021 (table 1dp → 8.2)
    REQUIRE_THAT(feet, WithinAbs(8.2021, 1e-4));
}

TEST_CASE("test_registerUnit_convertAllIncludesNewUnit", "[domain][register]") {
    // Given: cubit 등록
    UnitCatalog catalog = bootstrap_default_three_units();
    catalog.registerUnit("cubit", 0.4572);
    const ConversionEngine engine(catalog);

    // When: meter:1 convertAll
    const auto all = engine.convertAll("meter", 1.0);

    // Then: cubit 포함 (4단위 - source = 3 targets)
    REQUIRE(all.size() == 3);
    REQUIRE(all.count("cubit") == 1);
}
