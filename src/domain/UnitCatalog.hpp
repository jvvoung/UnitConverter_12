#pragma once

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

class UnitCatalog {
public:
    void add(const std::string& name, double meters_per_unit);
    void registerUnit(const std::string& name, double meters_per_unit);
    bool has(const std::string& name) const;
    double metersPerUnit(const std::string& name) const;
    std::vector<std::string> unitNames() const;
    void clear();
    bool empty() const;

private:
    std::unordered_map<std::string, double> units_;
};

UnitCatalog bootstrap_default_three_units();

class DuplicateUnitError : public std::runtime_error {
public:
    explicit DuplicateUnitError(const std::string& unit);
};

class UnknownUnitError : public std::runtime_error {
public:
    explicit UnknownUnitError(const std::string& unit);
};

class InvalidMetersPerUnitError : public std::runtime_error {
public:
    InvalidMetersPerUnitError();
};
