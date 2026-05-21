#include "boundary/CliOutputFormatter.hpp"

#include <sstream>

std::string CliOutputFormatter::formatLine(const std::string& source_value_token,
                                           const std::string& source_unit,
                                           double target_value,
                                           const std::string& target_unit) {
    std::ostringstream line;
    line << source_value_token << ' ' << source_unit << " = " << target_value << ' '
         << target_unit;
    return line.str();
}
