#pragma once

#include "domain/ConversionEngine.hpp"
#include "domain/UnitCatalog.hpp"

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace golden_master {

inline std::string readTextFile(const std::filesystem::path& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        return {};
    }
    std::string content{std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
    content.erase(std::remove(content.begin(), content.end(), '\r'), content.end());
    return content;
}

inline void writeTextFile(const std::filesystem::path& path, const std::string& content) {
    std::filesystem::create_directories(path.parent_path());
    std::ofstream out(path, std::ios::binary);
    if (!out) {
        throw std::runtime_error("Failed to write golden master file: " + path.string());
    }
    out << content;
}

inline std::string formatSixDecimalPlaces(double value) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6) << value;
    return oss.str();
}

inline std::string formatConversionLine(const std::string& value_token, const std::string& unit,
                                        double target_value, const std::string& target_unit) {
    return value_token + " " + unit + " = " + formatSixDecimalPlaces(target_value) + " " +
           target_unit;
}

inline std::vector<std::string> goldenScenarioInputs() {
    return {"meter:2.5", "feet:1.0", "yard:1.0", "meter:0.0"};
}

inline std::pair<std::string, std::string> splitScenarioInput(const std::string& scenario) {
    const auto colon = scenario.find(':');
    if (colon == std::string::npos) {
        throw std::invalid_argument("Invalid golden scenario: " + scenario);
    }
    return {scenario.substr(0, colon), scenario.substr(colon + 1)};
}

inline std::vector<std::string> buildScenarioLinesFromEngine(const std::string& scenario) {
    const auto [unit, value_token] = splitScenarioInput(scenario);
    const double value = std::stod(value_token);

    std::vector<std::string> lines;
    if (value == 0.0) {
        for (const auto& target_unit : {"feet", "meter", "yard"}) {
            if (target_unit == unit) {
                continue;
            }
            lines.push_back(formatConversionLine(value_token, unit, 0.0, target_unit));
        }
        return lines;
    }

    const ConversionEngine engine(bootstrap_default_three_units());
    const auto conversions = engine.convertAll(unit, value);

    lines.reserve(conversions.size());
    for (const auto& entry : conversions) {
        lines.push_back(formatConversionLine(value_token, unit, entry.second, entry.first));
    }
    return lines;
}

inline std::string buildScenarioBlock(const std::string& scenario) {
    std::ostringstream block;
    block << "[" << scenario << "]\n";
    for (const auto& line : buildScenarioLinesFromEngine(scenario)) {
        block << line << "\n";
    }
    block << "---\n";
    return block.str();
}

inline std::string buildGoldenDocument() {
    std::ostringstream document;
    for (const auto& scenario : goldenScenarioInputs()) {
        document << buildScenarioBlock(scenario);
    }
    return document.str();
}

inline std::string captureCliStdout(const std::filesystem::path& cli_path,
                                    const std::string& input) {
    const auto temp_dir = std::filesystem::temp_directory_path() / "unit_converter_golden";
    std::filesystem::create_directories(temp_dir);

    const auto input_file = temp_dir / "input.txt";
    const auto output_file = temp_dir / "output.txt";

    {
        std::ofstream in(input_file);
        if (!in) {
            throw std::runtime_error("Failed to create temp input file");
        }
        in << input;
    }

#ifdef _WIN32
    const std::string command = "cmd /c \"\"" + cli_path.string() + "\" < \"" +
                                input_file.string() + "\" > \"" + output_file.string() + "\"\"";
#else
    const std::string command = "\"" + cli_path.string() + "\" < \"" + input_file.string() +
                                "\" > \"" + output_file.string() + "\"";
#endif
    std::system(command.c_str());

    if (!std::filesystem::exists(output_file)) {
        throw std::runtime_error("CLI capture produced no output file");
    }

    const std::string captured = readTextFile(output_file);
    if (captured.empty()) {
        throw std::runtime_error("CLI capture produced empty stdout");
    }
    return captured;
}

inline std::string stripCarriageReturn(std::string line) {
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }
    return line;
}

inline std::string normalizeCliConversionLine(const std::string& line,
                                              const std::string& value_token,
                                              const std::string& source_unit) {
    const std::string normalized_line = stripCarriageReturn(line);
    const auto equals_pos = normalized_line.find(" = ");
    const auto trailing_unit_pos = normalized_line.rfind(' ');
    if (equals_pos == std::string::npos || trailing_unit_pos == std::string::npos ||
        trailing_unit_pos <= equals_pos + 3) {
        throw std::runtime_error("Invalid CLI conversion line: " + normalized_line);
    }

    const std::string target_unit = normalized_line.substr(trailing_unit_pos + 1);
    const double target_value =
        std::stod(normalized_line.substr(equals_pos + 3, trailing_unit_pos - equals_pos - 3));

    return formatConversionLine(value_token, source_unit, target_value, target_unit);
}

inline std::vector<std::string> parseCliConversionLines(const std::string& raw_stdout,
                                                        const std::string& value_token,
                                                        const std::string& source_unit) {
    std::vector<std::string> lines;
    std::istringstream stream(raw_stdout);
    std::string line;
    while (std::getline(stream, line)) {
        line = stripCarriageReturn(line);
        if (line.find("Insert value for converting") != std::string::npos) {
            const auto prompt_end = line.find("): ");
            if (prompt_end == std::string::npos) {
                continue;
            }
            line = stripCarriageReturn(line.substr(prompt_end + 3));
        }
        if (line.empty()) {
            continue;
        }
        const auto equals_pos = line.find(" = ");
        if (equals_pos == std::string::npos) {
            continue;
        }
        const auto trailing_unit_pos = line.rfind(' ');
        if (trailing_unit_pos == std::string::npos || trailing_unit_pos <= equals_pos) {
            continue;
        }
        const std::string target_unit = line.substr(trailing_unit_pos + 1);
        if (target_unit == source_unit) {
            continue;
        }
        lines.push_back(normalizeCliConversionLine(line, value_token, source_unit));
    }
    return lines;
}

inline std::string buildScenarioBlockFromCli(const std::filesystem::path& cli_path,
                                             const std::string& scenario) {
    const auto parsed = splitScenarioInput(scenario);
    const std::string& unit = parsed.first;
    const std::string& value_token = parsed.second;
    const std::string raw = captureCliStdout(cli_path, scenario);
    const auto parsed_lines = parseCliConversionLines(raw, value_token, unit);

    if (parsed_lines.empty()) {
        throw std::runtime_error("CLI produced no conversion lines for scenario: " + scenario);
    }

    std::ostringstream block;
    block << "[" << scenario << "]\n";
    for (const auto& line : parsed_lines) {
        block << line << "\n";
    }
    block << "---\n";
    return block.str();
}

inline std::string buildGoldenDocumentFromCli(const std::filesystem::path& cli_path) {
    std::ostringstream document;
    for (const auto& scenario : goldenScenarioInputs()) {
        document << buildScenarioBlockFromCli(cli_path, scenario);
    }
    return document.str();
}

inline std::vector<std::string> splitLines(const std::string& text) {
    std::vector<std::string> lines;
    std::istringstream stream(text);
    std::string line;
    while (std::getline(stream, line)) {
        line = stripCarriageReturn(line);
        lines.push_back(line);
    }
    return lines;
}

inline std::string extractSection(const std::string& document, const std::string& scenario) {
    const std::string header = "[" + scenario + "]\n";
    const auto start = document.find(header);
    if (start == std::string::npos) {
        throw std::runtime_error("Golden master section not found: " + scenario);
    }

    const auto end = document.find("---\n", start);
    if (end == std::string::npos) {
        throw std::runtime_error("Golden master section terminator missing: " + scenario);
    }

    return document.substr(start, end - start + 4);
}

inline std::string makeUnifiedDiff(const std::string& expected, const std::string& actual) {
    if (expected == actual) {
        return {};
    }

    const auto expected_lines = splitLines(expected);
    const auto actual_lines = splitLines(actual);
    const std::size_t max_lines = std::max(expected_lines.size(), actual_lines.size());

    std::ostringstream diff;
    diff << "--- expected\n";
    diff << "+++ actual\n";
    diff << "@@ -1," << expected_lines.size() << " +1," << actual_lines.size() << " @@\n";

    for (std::size_t index = 0; index < max_lines; ++index) {
        const bool has_expected = index < expected_lines.size();
        const bool has_actual = index < actual_lines.size();

        if (has_expected && has_actual && expected_lines[index] == actual_lines[index]) {
            diff << " " << expected_lines[index] << "\n";
            continue;
        }
        if (has_expected) {
            diff << "-" << expected_lines[index] << "\n";
        }
        if (has_actual) {
            diff << "+" << actual_lines[index] << "\n";
        }
    }

    return diff.str();
}

inline void assertScenarioMatchesExpected(const std::filesystem::path& expected_path,
                                          const std::filesystem::path& cli_path,
                                          const std::string& scenario,
                                          const std::string& test_id) {
    if (!std::filesystem::exists(expected_path)) {
        throw std::runtime_error("Golden master baseline missing: " + expected_path.string());
    }

    const std::string expected_document = readTextFile(expected_path);
    const std::string expected_section = extractSection(expected_document, scenario);
    const std::string actual_section = buildScenarioBlockFromCli(cli_path, scenario);

    if (expected_section == actual_section) {
        return;
    }

    const std::string diff = makeUnifiedDiff(expected_section, actual_section);
    throw std::runtime_error(test_id + " [" + scenario + "] golden master mismatch.\n" + diff);
}

inline std::filesystem::path defaultExpectedPath() {
#ifdef UNIT_CONVERTER_SOURCE_DIR
    return std::filesystem::path(UNIT_CONVERTER_SOURCE_DIR) / "tests" /
           "golden_master_expected.txt";
#else
    return std::filesystem::path("tests") / "golden_master_expected.txt";
#endif
}

inline std::filesystem::path defaultCliPath() {
#ifdef UNIT_CONVERTER_CLI_PATH
    return std::filesystem::path(UNIT_CONVERTER_CLI_PATH);
#else
    return std::filesystem::path("unit_converter_cli");
#endif
}

inline void approveGoldenMaster(const std::filesystem::path& expected_path,
                                const std::string& actual,
                                bool auto_create_baseline) {
    if (!std::filesystem::exists(expected_path)) {
        if (!auto_create_baseline) {
            throw std::runtime_error("Golden master baseline missing: " + expected_path.string());
        }
        writeTextFile(expected_path, actual);
        return;
    }

    const std::string expected = readTextFile(expected_path);
    if (expected == actual) {
        return;
    }

    const std::string diff = makeUnifiedDiff(expected, actual);
    throw std::runtime_error("Golden master mismatch.\nDiff:\n" + diff);
}

}  // namespace golden_master
