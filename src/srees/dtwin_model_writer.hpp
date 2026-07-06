#pragma once

#include "srees/matrix_adapter.hpp"
#include "srees/power_case.hpp"
#include "srees/solar_inverter.hpp"

#include <string>

namespace srees
{
struct DtwinPlotOptions
{
    bool includePowerPlot {true};
    bool includePerUnitPlot {true};
};

void writeDtwinModel(const std::string& outFileName,
                     const SolarInverterConfig& config,
                     const PowerCase& powerCase,
                     const SolarInverterResult& result,
                     const MatrixBuildSummary& matrices,
                     const DtwinPlotOptions& plotOptions = {});
}
