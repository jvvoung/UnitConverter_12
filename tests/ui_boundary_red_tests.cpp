#include <catch2/catch_test_macros.hpp>

#include "red_ui_contract.hpp"

#include <regex>
#include <string>

// Track A — UI / Boundary RED
// Current: UnitConverter.cpp single main(), no std::invalid_argument
// Expected: ConversionApplicationService unimplemented -> link FAIL (RED)

TEST_CASE("TC-A-01: meter:2.5 returns conversion result", "[ui][red]") {
    // Given: valid CONVERT input "meter:2.5", 1 meter = 3.28084 feet
    ConversionApplicationService svc;

    // When: convert
    const std::string result = svc.convert("meter:2.5");

    // Then: conversion result string (6 decimal feet)
    REQUIRE(result == "2.5 meter = 8.202100 feet");
}

TEST_CASE("TC-A-02: input without colon throws invalid_argument", "[ui][red]") {
    // Given: malformed input without ':'
    ConversionApplicationService svc;

    // When/Then: std::invalid_argument
    REQUIRE_THROWS_AS(svc.convert("meter2.5"), std::invalid_argument);
}

TEST_CASE("TC-A-03: negative value throws invalid_argument", "[ui][red]") {
    // Given: POL-NEG violation (negative value)
    ConversionApplicationService svc;

    // When/Then: std::invalid_argument
    REQUIRE_THROWS_AS(svc.convert("meter:-1.0"), std::invalid_argument);
}

TEST_CASE("TC-A-04: unknown unit throws invalid_argument", "[ui][red]") {
    // Given: unit not in catalog
    ConversionApplicationService svc;

    // When/Then: std::invalid_argument
    REQUIRE_THROWS_AS(svc.convert("parsec:1.0"), std::invalid_argument);
}

TEST_CASE("TC-A-05: output preserves source unit and value token", "[ui][red]") {
    // Given: meter:2.5 — POL-OUT source preservation
    ConversionApplicationService svc;

    // When: convert
    const std::string result = svc.convert("meter:2.5");

    // Then: "2.5 meter" prefix preserved
    REQUIRE(result.find("2.5 meter =") == 0);
    REQUIRE(result.find("meter") != std::string::npos);
}

TEST_CASE("TC-A-06: JSON format returns well-formed schema", "[ui][red]") {
    // Given: meter:2.5, JSON output (PRD 6.3)
    ConversionApplicationService svc;

    // When: convert(..., OutputFormat::Json)
    const std::string json = svc.convert("meter:2.5", OutputFormat::Json);

    // Then: source + conversions[] schema
    REQUIRE(std::regex_search(json, std::regex(R"("source"\s*:\s*\{)")));
    REQUIRE(std::regex_search(json, std::regex(R"("unit"\s*:\s*"meter")")));
    REQUIRE(std::regex_search(json, std::regex(R"("value"\s*:\s*2\.5)")));
    REQUIRE(std::regex_search(json, std::regex(R"("conversions"\s*:\s*\[)")));
    REQUIRE(std::regex_search(json, std::regex(R"("unit"\s*:\s*"feet")")));
    REQUIRE(std::regex_search(json, std::regex(R"("unit"\s*:\s*"yard")")));
}
