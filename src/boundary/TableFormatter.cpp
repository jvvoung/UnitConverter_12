#include "boundary/TableFormatter.hpp"

#include <cmath>
#include <iomanip>
#include <sstream>

double TableFormatter::roundToOneDecimalHalfUp(double value) {
    return std::round(value * 10.0) / 10.0;
}

std::string TableFormatter::formatLine(const std::string& source_value_token,
                                     const std::string& source_unit,
                                     double target_value,
                                     const std::string& target_unit) {
    const double rounded = roundToOneDecimalHalfUp(target_value);
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << rounded;
    const std::string target_display = oss.str();
    return source_value_token + " " + source_unit + " = " + target_display + " " + target_unit;
}
