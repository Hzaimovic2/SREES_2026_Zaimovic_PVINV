#include "srees/dtwin_model_writer.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <stdexcept>

namespace srees
{
namespace
{
std::string sanitizeName(std::string value)
{
    std::replace(value.begin(), value.end(), '"', '\'');
    return value;
}

void writeDigitalModel(const std::string& outFileName,
                       const SolarInverterConfig& config,
                       const PowerCase& powerCase,
                       const SolarInverterResult& result,
                       const MatrixBuildSummary& matrices)
{
    std::ofstream out(outFileName);
    if (!out)
        throw std::runtime_error("Cannot create output dmodl file");

    const auto modelName = sanitizeName(config.name);
    out << "Header:\n";
    out << "\tmaxIter = 20\n";
    out << "\treport = Solver\n";
    out << "\tmaxReps = -1\n";
    out << "\toutToTxt = false\n";
    out << "\tstartTime = 0\n";
    out << "\tdTime = 0.001\n";
    out << "\tendTime = 2\n";
    out << "end\n\n";
    out << "// Model created by SREES PV inverter converter\n";
    out << "// Case: " << powerCase.name << ", buses: " << powerCase.busCount << "\n";
    out << "// Matrix backend: " << matrices.backend << "\n\n";
    out << "Model [type=DAE domain=real method=RK2 name=\"" << modelName << "\"]:\n";
    out << "Vars [out=true]:\n";
    out << "\tp_ac = 0\n";
    out << "\tq_ac = 0\n";
    out << "\tp_dc = 0\n";
    out << "\tirradiance = " << config.irradianceWm2 << "\n";
    out << "\tcell_temp = " << config.cellTemperatureC << "\n";
    out << "Params:\n";
    out << "\tp_ac_ref = " << result.activePowerMw << "\n";
    out << "\tq_ac_ref = " << result.reactivePowerMvar << "\n";
    out << "\tp_dc_ref = " << result.dcPowerMw << "\n";
    out << "\tinverter_tau = 0.12\n";
    out << "\tconnection_bus = " << config.connectionBus << "\n";
    out << "\tdc_rated_mw = " << config.dcRatedPowerMw << "\n";
    out << "\tac_rated_mw = " << config.acRatedPowerMw << "\n";
    out << "\tpower_factor = " << config.powerFactor << "\n";
    out << "\tdense_rows = " << matrices.denseRows << "\n";
    out << "\tdense_cols = " << matrices.denseColumns << "\n";
    out << "\tsparse_rows = " << matrices.sparseRows << "\n";
    out << "\tsparse_cols = " << matrices.sparseColumns << "\n";
    out << "\tsparse_nonzeros = " << matrices.sparseNonZeros << "\n";
    out << "ODEs:\n";
    out << "\tp_ac' = (p_ac_ref - p_ac) / inverter_tau\n";
    out << "\tq_ac' = (q_ac_ref - q_ac) / inverter_tau\n";
    out << "\tp_dc' = (p_dc_ref - p_dc) / inverter_tau\n";
    out << "\tirradiance' = 0\n";
    out << "\tcell_temp' = 0\n";
    out << "end\n";
}

void writeVisualModel(const std::string& outFileName, const DtwinPlotOptions& plotOptions)
{
    auto visualPath = std::filesystem::path(outFileName);
    visualPath.replace_extension(".vmodl");

    std::ofstream out(visualPath);
    if (!out)
        return;

    out << "Header:\n";
    out << "\tnewTab = false\n";
    out << "\tdrawPlots = true\n";
    out << "end\n";
    out << "Model [name=\"Solar inverter visualization\"]:\n";
    out << "Plots [backColor=auto]:\n";
    out << "\tlinePlot [xLabel=\"Time [s]\" yLabel=\"Power [MW/MVAr]\" name=\"PV inverter\" legend=true]:\n";
    out << "\t\t@x << t\n";
    out << "\t\t@y << p_ac [colorL=darkGreen colorD=green width=2 name=\"Pac\"]\n";
    out << "\t\t@y << q_ac [colorL=darkBlue colorD=cyan width=2 name=\"Reactive\"]\n";
    out << "\t\t@y << p_dc [colorL=darkRed colorD=red width=2 name=\"Pdc\"]\n";
    out << "\tend\n";
    if (plotOptions.includePerUnitPlot)
    {
        out << "\tlinePlot [xLabel=\"Time [s]\" yLabel=\"p.u.\" name=\"PV inverter pu\" legend=true]:\n";
        out << "\t\t@x << t\n";
        out << "\t\t@y << 0.98 * p_ac / ac_rated_mw [colorL=darkGreen colorD=green width=2 name=\"Pac pu\"]\n";
        out << "\t\t@y << 0.98 * q_ac / ac_rated_mw [colorL=darkBlue colorD=cyan width=2 name=\"Reactive pu\"]\n";
        out << "\t\t@y << 0.98 * p_dc / dc_rated_mw [colorL=darkRed colorD=red width=2 name=\"Pdc pu\"]\n";
        out << "\tend\n";
    }
    out << "end\n";
}
}

void writeDtwinModel(const std::string& outFileName,
                     const SolarInverterConfig& config,
                     const PowerCase& powerCase,
                     const SolarInverterResult& result,
                     const MatrixBuildSummary& matrices,
                     const DtwinPlotOptions& plotOptions)
{
    writeDigitalModel(outFileName, config, powerCase, result, matrices);
    writeVisualModel(outFileName, plotOptions);
}
}
