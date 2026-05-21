#pragma once

// RED contract — UnitConverterLogic (no impl, Track B)

#include <map>
#include <string>

class UnitConverterLogic {
public:
    static void resetToDefaults();

    static double convert(const std::string& from_unit, double value, const std::string& to_unit);

    static std::map<std::string, double> convertAll(const std::string& from_unit, double value);

    static void registerUnit(const std::string& name, double ratio_to_meter);

    static void loadConfig(const std::string& path);
};
