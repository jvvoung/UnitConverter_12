#pragma once

#include "domain/UnitCatalog.hpp"

#include <string>

class ConfigLoadError : public std::runtime_error {
public:
    explicit ConfigLoadError(const std::string& path);
    const std::string& path() const { return path_; }

private:
    std::string path_;
};

class ConfigLoader {
public:
    static void loadFromFile(const std::string& path, UnitCatalog& catalog);
    static void loadFromJsonString(const std::string& json, UnitCatalog& catalog);
    static void loadFromYamlString(const std::string& yaml, UnitCatalog& catalog);
};
