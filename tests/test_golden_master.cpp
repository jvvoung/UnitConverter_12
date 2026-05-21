#include <catch2/catch_test_macros.hpp>

#include "golden_master.hpp"

#include <filesystem>
#include <string>
#include <vector>

class GoldenMasterTest {
protected:
    GoldenMasterTest() {
        cli_path_ = resolveCliPath();
        expected_path_ = golden_master::defaultExpectedPath();
        REQUIRE(std::filesystem::exists(cli_path_));
        REQUIRE(std::filesystem::exists(expected_path_));
    }

    std::string readExpectedSection(const std::string& scenario) const {
        const std::string expected_document = golden_master::readTextFile(expected_path_);
        return golden_master::extractSection(expected_document, scenario);
    }

    std::string captureActualSection(const std::string& scenario) const {
        return golden_master::buildScenarioBlockFromCli(cli_path_, scenario);
    }

    void expectSectionEquals(const std::string& scenario, const std::string& test_id) const {
        const std::string expected_str = readExpectedSection(scenario);
        const std::string actual_str = captureActualSection(scenario);

        if (expected_str != actual_str) {
            const std::string diff = golden_master::makeUnifiedDiff(expected_str, actual_str);
            FAIL(test_id << " [" << scenario << "] golden master mismatch.\n" << diff);
        }
    }

private:
    static std::filesystem::path resolveCliPath() {
        const auto configured = golden_master::defaultCliPath();
        if (std::filesystem::exists(configured)) {
            return configured;
        }

        std::vector<std::filesystem::path> candidates = {
            std::filesystem::current_path() / "unit_converter_cli.exe",
            std::filesystem::current_path() / "unit_converter_cli",
            std::filesystem::current_path() / "Debug" / "unit_converter_cli.exe",
            std::filesystem::current_path() / "Release" / "unit_converter_cli.exe",
            std::filesystem::path("unit_converter_cli.exe"),
            std::filesystem::path("unit_converter_cli"),
            std::filesystem::path("Debug") / "unit_converter_cli.exe",
            std::filesystem::path("Release") / "unit_converter_cli.exe",
            std::filesystem::path("..") / "unit_converter_cli.exe",
            std::filesystem::path("..") / "unit_converter_cli",
            std::filesystem::path("..") / "Debug" / "unit_converter_cli.exe",
            std::filesystem::path("..") / "Release" / "unit_converter_cli.exe",
        };

#ifdef UNIT_CONVERTER_BUILD_DIR
        const std::filesystem::path build_dir(UNIT_CONVERTER_BUILD_DIR);
        candidates.push_back(build_dir / "unit_converter_cli.exe");
        candidates.push_back(build_dir / "Debug" / "unit_converter_cli.exe");
        candidates.push_back(build_dir / "Release" / "unit_converter_cli.exe");
#endif

        for (const auto& candidate : candidates) {
            if (std::filesystem::exists(candidate)) {
                return candidate;
            }
        }

        return configured;
    }

    std::filesystem::path cli_path_;
    std::filesystem::path expected_path_;
};

TEST_CASE_METHOD(GoldenMasterTest, "UnitConverter_meter_2_5", "[GoldenMaster][GM-TC-01]") {
    expectSectionEquals("meter:2.5", "GM-TC-01");
}

TEST_CASE_METHOD(GoldenMasterTest, "UnitConverter_feet_1_0", "[GoldenMaster][GM-TC-02]") {
    expectSectionEquals("feet:1.0", "GM-TC-02");
}

TEST_CASE_METHOD(GoldenMasterTest, "UnitConverter_yard_1_0", "[GoldenMaster][GM-TC-03]") {
    expectSectionEquals("yard:1.0", "GM-TC-03");
}

TEST_CASE_METHOD(GoldenMasterTest, "UnitConverter_meter_0_0", "[GoldenMaster][GM-TC-04]") {
    expectSectionEquals("meter:0.0", "GM-TC-04");
}
