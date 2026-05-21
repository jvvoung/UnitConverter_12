#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "DecayingUnit.hpp"

#include <stdexcept>

using Catch::Matchers::WithinAbs;

namespace {

constexpr double kCubitMetersPerUnit = 0.4572;
constexpr double kMeterToFeet = 3.28084;
constexpr double kTolerance = 1e-5;

DecayingUnit makeConverterWithCubit() {
    DecayingUnit converter;
    converter.registerUnit("cubit", kCubitMetersPerUnit);
    return converter;
}

}  // namespace

TEST_CASE("감쇠 단위 변환", "[bonus]") {
    SECTION("BT-01: registerUnit 후 cubit→meter") {
        DecayingUnit converter = makeConverterWithCubit();
        REQUIRE_THAT(converter.convert("cubit", 1.0, "meter"),
                     WithinAbs(kCubitMetersPerUnit, kTolerance));
    }

    SECTION("BT-02: registerUnit 후 meter→cubit 역변환") {
        DecayingUnit converter = makeConverterWithCubit();
        const double expected = 1.0 / kCubitMetersPerUnit;
        REQUIRE_THAT(converter.convert("meter", 1.0, "cubit"), WithinAbs(expected, kTolerance));
    }

    SECTION("BT-03: registerUnit 후 cubit→feet 교차 변환") {
        DecayingUnit converter = makeConverterWithCubit();
        const double expected = kCubitMetersPerUnit * kMeterToFeet;
        REQUIRE_THAT(converter.convert("cubit", 1.0, "feet"), WithinAbs(expected, kTolerance));
    }

    SECTION("BT-04: 음수 비율 등록 시 std::invalid_argument") {
        DecayingUnit converter;
        REQUIRE_THROWS_AS(converter.registerUnit("bad", -0.4572), std::invalid_argument);
    }

    SECTION("BT-05: registerUnit 후 convertAll 전 단위 반환") {
        DecayingUnit converter = makeConverterWithCubit();
        const auto all = converter.convertAll("cubit", 1.0);
        REQUIRE(all.size() == 3);
        REQUIRE(all.count("meter") == 1);
        REQUIRE(all.count("feet") == 1);
        REQUIRE(all.count("yard") == 1);
        REQUIRE_THAT(all.at("meter"), WithinAbs(kCubitMetersPerUnit, kTolerance));
    }

    SECTION("BT-06: 기존 meter→feet 결과 불변 (회귀 보호)") {
        DecayingUnit converter = makeConverterWithCubit();
        REQUIRE_THAT(converter.convert("meter", 1.0, "feet"),
                     WithinAbs(kMeterToFeet, kTolerance));
    }
}
