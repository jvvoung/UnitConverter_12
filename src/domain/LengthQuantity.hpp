#pragma once

#include <stdexcept>
#include <string>

class LengthQuantity {
public:
    static LengthQuantity create(const std::string& unit, double value);

    const std::string& unit() const { return unit_; }
    double value() const { return value_; }

private:
    LengthQuantity(std::string unit, double value);
    std::string unit_;
    double value_;
};

class InvalidLengthValueError : public std::runtime_error {
public:
    explicit InvalidLengthValueError(double value);
};
