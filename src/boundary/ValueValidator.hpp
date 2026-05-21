#pragma once

#include <string>

struct ValueValidationResult {
    bool ok = false;
    std::string error_message;
};

class ValueValidator {
public:
    static ValueValidationResult validatePositive(double value, const std::string& value_token);
};
