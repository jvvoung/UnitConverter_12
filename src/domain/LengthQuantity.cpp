#include "domain/LengthQuantity.hpp"

#include <cmath>
#include <sstream>

LengthQuantity LengthQuantity::create(const std::string& unit, double value) {
    if (!(value > 0.0) || !std::isfinite(value)) {
        throw InvalidLengthValueError(value);
    }
    return LengthQuantity(unit, value);
}

LengthQuantity::LengthQuantity(std::string unit, double value)
    : unit_(std::move(unit)), value_(value) {}

InvalidLengthValueError::InvalidLengthValueError(double value)
    : std::runtime_error([&]() {
          std::ostringstream oss;
          oss << "Value must be positive: " << value;
          return oss.str();
      }()) {}
