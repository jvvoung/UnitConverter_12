#include "red_logic_contract.hpp"

#include "domain/ConversionEngine.hpp"
#include "domain/UnitCatalog.hpp"

namespace {

UnitCatalog& logicCatalog() {
    static UnitCatalog catalog = bootstrap_default_three_units();
    return catalog;
}

ConversionEngine logicEngine() {
    return ConversionEngine(logicCatalog());
}

}  // namespace

void UnitConverterLogic::resetToDefaults() {
    logicCatalog() = bootstrap_default_three_units();
}

double UnitConverterLogic::convert(const std::string& from_unit, double value,
                                   const std::string& to_unit) {
    return logicEngine().convert(from_unit, value, to_unit);
}

std::map<std::string, double> UnitConverterLogic::convertAll(const std::string& from_unit,
                                                             double value) {
    return logicEngine().convertAll(from_unit, value);
}

void UnitConverterLogic::registerUnit(const std::string& name, double ratio_to_meter) {
    logicCatalog().registerUnit(name, ratio_to_meter);
}

void UnitConverterLogic::loadConfig(const std::string& /*path*/) {}
