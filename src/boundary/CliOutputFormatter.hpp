#pragma once

#include <string>

class CliOutputFormatter {
public:
    static std::string formatLine(const std::string& source_value_token,
                                  const std::string& source_unit,
                                  double target_value,
                                  const std::string& target_unit);
};
