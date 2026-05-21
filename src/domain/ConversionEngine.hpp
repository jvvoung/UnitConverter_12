#pragma once

#include "domain/LengthQuantity.hpp"
#include "domain/UnitCatalog.hpp"

#include <map>
#include <string>

class ConversionEngine {
public:
    explicit ConversionEngine(const UnitCatalog& catalog);

    double convert(const std::string& from_unit, double value, const std::string& to_unit) const;
    double convert(const LengthQuantity& source, const std::string& to_unit) const;
    std::map<std::string, double> convertAll(const std::string& from_unit, double value) const;

private:
    const UnitCatalog& catalog_;
};
