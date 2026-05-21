#include <catch2/catch_test_macros.hpp>

#include "red_ui_contract.hpp"

// Track A - UI / Boundary (ConversionApplicationService)

TEST_CASE("valid_input_meter_colon_value_returns_conversion_result", "[ui][red]") {
    FAIL("RED");
}

TEST_CASE("input_without_colon_throws_invalid_argument", "[ui][green]") {
    ConversionApplicationService service;
    REQUIRE_THROWS_AS(service.convert("meter2.5"), std::invalid_argument);
}

TEST_CASE("negative_value_throws_invalid_argument", "[ui][green]") {
    ConversionApplicationService service;
    REQUIRE_THROWS_AS(service.convert("meter:-1.0"), std::invalid_argument);
}

TEST_CASE("unknown_unit_throws_invalid_argument", "[ui][red]") {
    FAIL("RED");
}

TEST_CASE("output_preserves_source_unit_and_value", "[ui][red]") {
    FAIL("RED");
}

TEST_CASE("json_format_returns_valid_schema", "[ui][red]") {
    FAIL("RED");
}
