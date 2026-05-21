#pragma once

#include <map>
#include <memory>
#include <string>

class DecayingUnit {
public:
    DecayingUnit();
    ~DecayingUnit();

    DecayingUnit(const DecayingUnit&) = delete;
    DecayingUnit& operator=(const DecayingUnit&) = delete;
    DecayingUnit(DecayingUnit&&) noexcept;
    DecayingUnit& operator=(DecayingUnit&&) noexcept;

    void registerUnit(const std::string& name, double meters_per_unit);
    double convert(const std::string& from_unit, double value, const std::string& to_unit) const;
    std::map<std::string, double> convertAll(const std::string& from_unit, double value) const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
