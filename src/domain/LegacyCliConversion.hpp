#pragma once

#include "domain/UnitCatalog.hpp"

#include <map>
#include <string>
#include <vector>

double legacyCliConvert(const UnitCatalog& catalog, const std::string& from_unit, double value,
                        const std::string& to_unit);

std::map<std::string, double> legacyCliConvertAll(const UnitCatalog& catalog,
                                                  const std::string& from_unit, double value);

std::vector<std::string> legacyCliDisplayUnitOrder();
