#include "data/ConfigLoader.hpp"

#include <cmath>
#include <fstream>
#include <regex>
#include <sstream>

namespace {

std::string readFile(const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        throw ConfigLoadError(path);
    }
    std::ostringstream buffer;
    buffer << in.rdbuf();
    return buffer.str();
}

bool extractIntField(const std::string& json, const std::string& key, int& out) {
    const std::regex pattern("\"" + key + "\"\\s*:\\s*(-?\\d+)");
    std::smatch match;
    if (std::regex_search(json, match, pattern)) {
        out = std::stoi(match[1].str());
        return true;
    }
    return false;
}

bool extractStringField(const std::string& json, const std::string& key, std::string& out) {
    const std::regex pattern("\"" + key + "\"\\s*:\\s*\"([^\"]+)\"");
    std::smatch match;
    if (std::regex_search(json, match, pattern)) {
        out = match[1].str();
        return true;
    }
    return false;
}

struct UnitEntry {
    std::string name;
    double meters_per_unit = 0.0;
};

std::vector<UnitEntry> extractUnits(const std::string& json) {
    std::vector<UnitEntry> units;
    const std::regex unit_block(
        "\\{\\s*\"name\"\\s*:\\s*\"([^\"]+)\"\\s*,\\s*\"meters_per_unit\"\\s*:\\s*([0-9.eE+-]+)\\s*\\}");
    auto begin = std::sregex_iterator(json.begin(), json.end(), unit_block);
    auto end = std::sregex_iterator();
    for (auto it = begin; it != end; ++it) {
        UnitEntry entry;
        entry.name = (*it)[1].str();
        entry.meters_per_unit = std::stod((*it)[2].str());
        units.push_back(entry);
    }
    return units;
}

void applyUnits(const std::vector<UnitEntry>& units, UnitCatalog& catalog) {
    catalog.clear();
    for (const auto& unit : units) {
        catalog.add(unit.name, unit.meters_per_unit);
    }
}

void validateSchema(const std::string& content, bool is_json) {
    int schema_version = 0;
    std::string base_unit;
    if (is_json) {
        if (!extractIntField(content, "schema_version", schema_version) ||
            !extractStringField(content, "base_unit", base_unit)) {
            throw std::runtime_error("invalid schema");
        }
    } else {
        const std::regex version_re("schema_version\\s*:\\s*(\\d+)");
        const std::regex base_re("base_unit\\s*:\\s*(\\w+)");
        std::smatch match;
        if (!std::regex_search(content, match, version_re)) {
            throw std::runtime_error("invalid schema");
        }
        schema_version = std::stoi(match[1].str());
        if (!std::regex_search(content, match, base_re)) {
            throw std::runtime_error("invalid schema");
        }
        base_unit = match[1].str();
    }

    if (schema_version != 1 || base_unit != "meter") {
        throw std::runtime_error("invalid schema");
    }
}

}  // namespace

ConfigLoadError::ConfigLoadError(const std::string& path)
    : std::runtime_error("Failed to load config: " + path), path_(path) {}

void ConfigLoader::loadFromFile(const std::string& path, UnitCatalog& catalog) {
    const std::string content = readFile(path);
    if (path.size() >= 5 &&
        (path.substr(path.size() - 5) == ".yaml" || path.substr(path.size() - 4) == ".yml")) {
        loadFromYamlString(content, catalog);
        return;
    }
    loadFromJsonString(content, catalog);
}

void ConfigLoader::loadFromJsonString(const std::string& json, UnitCatalog& catalog) {
    validateSchema(json, true);
    const auto units = extractUnits(json);
    if (units.empty()) {
        throw std::runtime_error("empty units");
    }
    applyUnits(units, catalog);
}

void ConfigLoader::loadFromYamlString(const std::string& yaml, UnitCatalog& catalog) {
    validateSchema(yaml, false);
    std::vector<UnitEntry> units;
    const std::regex unit_line(
        "-\\s*name\\s*:\\s*(\\w+)\\s*\\n\\s*meters_per_unit\\s*:\\s*([0-9.eE+-]+)");
    auto begin = std::sregex_iterator(yaml.begin(), yaml.end(), unit_line);
    auto end = std::sregex_iterator();
    for (auto it = begin; it != end; ++it) {
        UnitEntry entry;
        entry.name = (*it)[1].str();
        entry.meters_per_unit = std::stod((*it)[2].str());
        units.push_back(entry);
    }
    if (units.empty()) {
        throw std::runtime_error("empty units");
    }
    applyUnits(units, catalog);
}
