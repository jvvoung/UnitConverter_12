#include <catch2/catch_test_macros.hpp>

#include "boundary/InputParser.hpp"
#include "boundary/ValueValidator.hpp"
#include "domain/ConversionEngine.hpp"
#include "domain/LengthQuantity.hpp"
#include "domain/UnitCatalog.hpp"
#include "test_helpers.hpp"

// 예외 — 잘못된 형식 / 음수 / 없는 단위

TEST_CASE("test_inputParser_missingColon_invalidFormatMessage", "[boundary][exception]") {
    // Given: CONVERT 형식 unit:value 필수
    // When: "meter2.5" (콜론 없음)
    const auto result = InputParser::parseConvertLine("meter2.5");

    // Then: INVALID_FORMAT
    REQUIRE_FALSE(result.ok);
    REQUIRE(result.error_message ==
            "Invalid format. Use unit:value (ex: meter:2.5)");
}

TEST_CASE("test_inputParser_nonNumericToken_invalidNumberMessage", "[boundary][exception]") {
    // Given: value는 유한 십진
    // When: "meter:abc"
    const auto result = InputParser::parseConvertLine("meter:abc");

    // Then: INVALID_NUMBER
    REQUIRE_FALSE(result.ok);
    REQUIRE(result.error_message == "Invalid number: abc");
}

TEST_CASE("test_valueValidator_negativeValue_positiveRequiredMessage", "[boundary][exception]") {
    // Given: POL-NEG — value ≤ 0 거부
    // When: -2.5 검증
    const auto result = ValueValidator::validatePositive(-2.5, "-2.5");

    // Then: Value must be positive: -2.5
    REQUIRE_FALSE(result.ok);
    REQUIRE(result.error_message == "Value must be positive: -2.5");
}

TEST_CASE("test_valueValidator_zeroValue_positiveRequiredMessage", "[boundary][exception]") {
    // Given: POL-NEG-01
    // When: 0 검증 (토큰 "0")
    const auto result = ValueValidator::validatePositive(0.0, "0");

    // Then: Value must be positive: 0
    REQUIRE_FALSE(result.ok);
    REQUIRE(result.error_message == "Value must be positive: 0");
}

TEST_CASE("test_convert_unknownUnit_throwsUnknownUnit", "[domain][exception]") {
    // Given: 기본 Catalog (meter, feet, yard)
    const auto catalog = bootstrap_default_three_units();
    const ConversionEngine engine(catalog);

    // When: parsec:1.0 환산 시도
    // Then: Unknown unit: parsec
    REQUIRE_THROWS_AS(engine.convert("parsec", 1.0, "meter"), UnknownUnitError);
}

TEST_CASE("test_lengthQuantity_negativeValue_rejectsCreation", "[domain][exception]") {
    // Given: Domain 불변식 — 양의 유한만
    // When: meter:-2.5
    // Then: InvalidLengthValueError
    REQUIRE_THROWS_AS(LengthQuantity::create("meter", -2.5), InvalidLengthValueError);
}

TEST_CASE("test_inputParser_emptyValueToken_invalidNumber", "[boundary][exception]") {
    // Given: value 토큰 비어 있음
    // When: "meter:"
    const auto result = InputParser::parseConvertLine("meter:");

    // Then: Invalid number: (빈 토큰)
    REQUIRE_FALSE(result.ok);
    REQUIRE(result.error_message == "Invalid number: ");
}

TEST_CASE("test_catalog_lookupMissingUnit_throwsUnknownUnit", "[domain][exception]") {
    // Given: Catalog에 stone 없음
    UnitCatalog catalog = bootstrap_default_three_units();

    // When/Then: Unknown unit: stone
    REQUIRE_THROWS_AS(catalog.metersPerUnit("stone"), UnknownUnitError);
}
