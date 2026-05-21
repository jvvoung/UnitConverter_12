#pragma once

#include "domain/ConversionEngine.hpp"
#include "domain/UnitCatalog.hpp"

#include <cmath>
#include <string>

inline bool nearEqual(double actual, double expected, double rel_eps = 1e-9) {
    if (!std::isfinite(actual) || !std::isfinite(expected)) {
        return false;
    }
    const double tolerance = std::max(1e-9, std::abs(expected) * rel_eps);
    return std::abs(actual - expected) <= tolerance;
}

inline std::string testDataPath(const std::string& filename) {
#ifdef UNIT_CONVERTER_TEST_DATA_DIR
    return std::string(UNIT_CONVERTER_TEST_DATA_DIR) + "/" + filename;
#else
    return std::string("testdata/") + filename;
#endif
}
