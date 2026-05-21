#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "data/ConfigLoader.hpp"
#include "domain/ConversionEngine.hpp"
#include "domain/UnitCatalog.hpp"
#include "test_helpers.hpp"

using Catch::Matchers::WithinAbs;

#ifndef UNIT_CONVERTER_TEST_DATA_DIR
#define UNIT_CONVERTER_TEST_DATA_DIR "testdata"
#endif

// 설정 로드 — JSON/YAML 정상·실패 케이스

TEST_CASE("test_loadConfig_validJson_populatesCatalog", "[data][config]") {
    // Given: schema_version=1, base_unit=meter, 3 units (1 m = 3.28084 ft)
    UnitCatalog catalog;
    const std::string path = testDataPath("units_valid.json");

    // When: JSON 로드
    ConfigLoader::loadFromFile(path, catalog);
    const ConversionEngine engine(catalog);

    // Then: meter:2.5 → feet ≈ 8.2021
    REQUIRE(catalog.has("feet"));
    REQUIRE_THAT(engine.convert("meter", 2.5, "feet"), WithinAbs(8.2021, 1e-4));
}

TEST_CASE("test_loadConfig_validYaml_populatesCatalog", "[data][config]") {
    // Given: YAML 동일 스키마
    UnitCatalog catalog;
    const std::string path = testDataPath("units_valid.yaml");

    // When: YAML 로드
    ConfigLoader::loadFromFile(path, catalog);
    const ConversionEngine engine(catalog);

    // Then: 1 meter = 1.09361 yard
    REQUIRE_THAT(engine.convert("meter", 1.0, "yard"), WithinAbs(1.09361, 1e-5));
}

TEST_CASE("test_loadConfig_missingFile_throwsConfigLoadError", "[data][config]") {
    // Given: 존재하지 않는 경로
    UnitCatalog catalog;

    // When/Then: Failed to load config: {path}, exit 2 계약
    REQUIRE_THROWS_AS(
        ConfigLoader::loadFromFile(testDataPath("no_such_file.json"), catalog),
        ConfigLoadError);
}

TEST_CASE("test_loadConfig_invalidBaseUnit_throwsRuntimeError", "[data][config]") {
    // Given: base_unit != meter
    UnitCatalog catalog;
    const std::string path = testDataPath("units_invalid_base.json");

    // When/Then: CONFIG_LOAD_FAILED (no fallback)
    REQUIRE_THROWS(ConfigLoader::loadFromFile(path, catalog));
}

TEST_CASE("test_loadConfig_emptyUnitsArray_throwsRuntimeError", "[data][config]") {
    // Given: units[] 빈 배열 금지
    UnitCatalog catalog;
    const std::string path = testDataPath("units_empty.json");

    // When/Then: 로드 실패
    REQUIRE_THROWS(ConfigLoader::loadFromFile(path, catalog));
}

TEST_CASE("test_loadConfig_malformedJson_throwsRuntimeError", "[data][config]") {
    // Given: 깨진 JSON
    UnitCatalog catalog;
    const std::string broken = R"({"schema_version": 1, "base_unit": "meter")";

    // When/Then: units 추출 실패 → 예외
    REQUIRE_THROWS(ConfigLoader::loadFromJsonString(broken, catalog));
}

TEST_CASE("test_loadConfig_jsonString_replacesDefaultCatalog", "[data][config]") {
    // Given: 커스텀 단위 only inch (0.0254 m)
    const std::string json = R"({
      "schema_version": 1,
      "base_unit": "meter",
      "units": [
        { "name": "meter", "meters_per_unit": 1.0 },
        { "name": "inch", "meters_per_unit": 0.0254 }
      ]
    })";
    UnitCatalog catalog = bootstrap_default_three_units();

    // When: InMemory JSON 로드
    ConfigLoader::loadFromJsonString(json, catalog);
    const ConversionEngine engine(catalog);

    // Then: feet 없음, inch 있음; 1 meter = 39.3700787 inch
    REQUIRE_FALSE(catalog.has("feet"));
    REQUIRE(catalog.has("inch"));
    REQUIRE_THAT(engine.convert("meter", 1.0, "inch"), WithinAbs(39.3700787, 1e-3));
}
