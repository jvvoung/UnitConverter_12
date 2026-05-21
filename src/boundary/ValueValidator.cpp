#include "boundary/ValueValidator.hpp"

#include <cmath>
#include <sstream>

ValueValidationResult ValueValidator::validatePositive(double value,
                                                       const std::string& value_token) {
    ValueValidationResult result;
    if (value > 0.0 && std::isfinite(value)) {
        result.ok = true;
        return result;
    }
    result.ok = false;
    std::ostringstream oss;
    oss << "Value must be positive: " << value_token;
    result.error_message = oss.str();
    return result;
}
