#include <catch2/catch_test_macros.hpp>

#include "golden_master.hpp"

#include <filesystem>

namespace {

std::filesystem::path resolveCliPath() {
    const auto configured = golden_master::defaultCliPath();
    if (std::filesystem::exists(configured)) {
        return configured;
    }

    const std::filesystem::path candidates[] = {
        std::filesystem::current_path() / "unit_converter_cli.exe",
        std::filesystem::current_path() / "unit_converter_cli",
        std::filesystem::path("unit_converter_cli.exe"),
        std::filesystem::path("unit_converter_cli"),
        std::filesystem::path("..") / "unit_converter_cli.exe",
        std::filesystem::path("..") / "unit_converter_cli",
    };

    for (const auto& candidate : candidates) {
        if (std::filesystem::exists(candidate)) {
            return candidate;
        }
    }

    return configured;
}

}  // namespace

TEST_CASE("golden_master_cli_regression_matches_expected", "[golden][regression][approval]") {
    const auto expected_path = golden_master::defaultExpectedPath();
    const auto cli_path = resolveCliPath();

    REQUIRE(std::filesystem::exists(cli_path));

    const std::string actual = golden_master::buildGoldenDocumentFromCli(cli_path);
    const bool auto_create = !std::filesystem::exists(expected_path);

    try {
        golden_master::approveGoldenMaster(expected_path, actual, auto_create);
    } catch (const std::exception& ex) {
        if (auto_create) {
            SUCCEED("Golden master baseline created at " << expected_path.string());
            return;
        }
        FAIL(ex.what());
    }

    if (auto_create) {
        SUCCEED("Golden master baseline created at " << expected_path.string());
    } else {
        SUCCEED("Golden master matches expected baseline");
    }
}

TEST_CASE("golden_master_engine_baseline_document", "[golden][regression]") {
    const std::string document = golden_master::buildGoldenDocument();

    REQUIRE(document.find("[meter:2.5]\n") != std::string::npos);
    REQUIRE(document.find("2.5 meter = 8.202100 feet\n") != std::string::npos);
    REQUIRE(document.find("[feet:1.0]\n") != std::string::npos);
    REQUIRE(document.find("[yard:1.0]\n") != std::string::npos);
    REQUIRE(document.find("[meter:0.0]\n") != std::string::npos);
}
