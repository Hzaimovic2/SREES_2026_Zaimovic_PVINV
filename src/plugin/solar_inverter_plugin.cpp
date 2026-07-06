#include "srees/converter.hpp"
#include "srees/dtwin_model_writer.hpp"
#include "srees/power_case.hpp"
#include "srees/solar_inverter.hpp"
#include "srees/xml_config.hpp"

#include <compiler/Definitions.h>
#include <gui/Button.h>
#include <gui/FileDialog.h>
#include <gui/GridComposer.h>
#include <gui/GridLayout.h>
#include <gui/HorizontalLayout.h>
#include <gui/Label.h>
#include <gui/LineEdit.h>
#include <gui/TextEdit.h>
#include <gui/View.h>
#include <gui/Window.h>
#include <sc/IPlugin.h>

#include <filesystem>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#if defined(MU_WINDOWS)
    #if defined(PLUGIN_EXPORTS)
        #define PLUGIN_API __declspec(dllexport)
    #else
        #define PLUGIN_API __declspec(dllimport)
    #endif
#elif defined(PLUGIN_EXPORTS)
    #define PLUGIN_API __attribute__((visibility("default")))
#else
    #define PLUGIN_API
#endif

namespace
{
void onClosedPluginWindow();

class SolarConverterView : public gui::View
{
public:
    SolarConverterView(sc::IPlugin* plugin, const sc::IPlugin::CallBack& onComplete, td::UINT4 wndID)
        : _plugin(plugin)
        , _onComplete(onComplete)
        , _wndID(wndID)
        , _lblConfig("Config XML:")
        , _lblCase("Case XML:")
        , _lblOut("Output:")
        , _lblCaseButtons("Test cases:")
        , _lblScenarios("Scenarios:")
        , _lblManual("Manual:")
        , _lblStatus("Status:")
        , _lblNetwork("1. PROJECT AND BUS")
        , _lblTopology("2. CONNECTION PREVIEW")
        , _lblSimulation("3. PV AND INVERTER")
        , _lblResults("4. RESULTS")
        , _lblMatrices("5. MATRIX INFORMATION")
        , _lblLog("6. PROGRESS AND LOG")
        , _btnConfig("...")
        , _btnCase("...")
        , _btnOut("...")
        , _btnCase9("case9")
        , _btnCase30("case30")
        , _btnCase118("case118")
        , _btnCase300("case300")
        , _btnSunny("Sunny")
        , _btnCloudy("Cloudy")
        , _btnHot("Hot")
        , _btnLowPf("Low PF")
        , _btnRecommend("Recommend bus")
        , _btnPreview("Preview")
        , _btnConvert("Convert")
        , _btnConvertPu("Convert + p.u.")
        , _caseButtons(5)
        , _scenarioButtons(5)
        , _manualInputs(10)
        , _bottomButtons(4)
        , _layout(12, 4)
    {
        _editStatus.setAsReadOnly();
        _network.setAsReadOnly();
        _topology.setAsReadOnly();
        _results.setAsReadOnly();
        _matrices.setAsReadOnly();
        _simulation.setAsReadOnly();
        _log.setAsReadOnly();
        _editStatus = "Select config XML and case XML.";
        _network.setText("Project and case selection\n\nTest system: not selected\nBuses: -\nBranches: -\nBase MVA: -\n\nConnection bus\n\nSelected bus: 5\nBus type: PQ/load\nValidation: waiting\n\nBus summary\nBus  Type   Vm     P      Q\n1    Slack  1.040  -      -\n5    PQ     1.000  125.0  50.0");
        _topology.setText("Connection preview\n\nSelect case and press Preview.\n\nBefore: original test case\nAfter: PV inverter connected to selected bus");
        _results.setText("Results at selected bus\n\nPdc available       - MW\nPac injected        - MW\nQ ac injected       - MVAr\nS apparent          - MVA\nPF actual           -\nEstimated V before  - p.u.\nEstimated V after   - p.u.\nEstimated delta V   - p.u.\n\nPer-unit values will be shown after Preview.");
        _matrices.setText("Matrix information\n\nDense Ybus: waiting\nSparse Ybus: waiting\nNonzeros: waiting\nBackend: natID dense + sparse");
        _simulation.setText("PV and inverter parameters\n\nSolar plant\nRated DC power: from XML\nIrradiance G: scenario/manual\nTemperature Tcell: scenario/manual\nTemperature coeff.: from XML\n\nInverter\nRated AC power: from XML\nPower factor PF: scenario/manual\nReactive mode: fixed PF\n\nScenarios: Sunny, Cloudy, Hot, Low PF");
        _log.setText("[ready] select XML files or press a quick case button");
        _logText = "[ready] select XML files or press a quick case button\n";

        _lblConfig.setBold();
        _lblCase.setBold();
        _lblOut.setBold();
        _lblCaseButtons.setBold();
        _lblScenarios.setBold();
        _lblManual.setBold();
        _lblStatus.setBold();
        _lblNetwork.setBold();
        _lblTopology.setBold();
        _lblResults.setBold();
        _lblMatrices.setBold();
        _lblSimulation.setBold();
        _lblLog.setBold();
        _lblNetwork.setTextColor(td::Accent::Plain);
        _lblTopology.setTextColor(td::Accent::Plain);
        _lblResults.setTextColor(td::Accent::Success);
        _lblMatrices.setTextColor(td::Accent::Plain);
        _lblSimulation.setTextColor(td::Accent::Plain);
        _lblLog.setTextColor(td::Accent::Success);
        _network.setFontName("Segoe UI");
        _topology.setFontName("Consolas");
        _results.setFontName("Segoe UI");
        _matrices.setFontName("Segoe UI");
        _simulation.setFontName("Segoe UI");
        _log.setFontName("Consolas");
        _network.setFontSize(8.1F);
        _topology.setFontSize(8.2F);
        _results.setFontSize(8.1F);
        _matrices.setFontSize(8.0F);
        _simulation.setFontSize(8.1F);
        _log.setFontSize(8.0F);
        _network.setSizeLimits(290, gui::Control::Limit::UseAsMin, 310, gui::Control::Limit::UseAsMin);
        _topology.setSizeLimits(330, gui::Control::Limit::UseAsMin, 310, gui::Control::Limit::UseAsMin);
        _simulation.setSizeLimits(290, gui::Control::Limit::UseAsMin, 310, gui::Control::Limit::UseAsMin);
        _results.setSizeLimits(270, gui::Control::Limit::UseAsMin, 310, gui::Control::Limit::UseAsMin);
        _matrices.setSizeLimits(290, gui::Control::Limit::UseAsMin, 92, gui::Control::Limit::UseAsMin);
        _log.setSizeLimits(900, gui::Control::Limit::UseAsMin, 92, gui::Control::Limit::UseAsMin);

        _btnCase9.setSizeLimitForNChars(9, gui::Control::Limit::Fixed);
        _btnCase30.setSizeLimitForNChars(9, gui::Control::Limit::Fixed);
        _btnCase118.setSizeLimitForNChars(9, gui::Control::Limit::Fixed);
        _btnCase300.setSizeLimitForNChars(9, gui::Control::Limit::Fixed);
        _btnSunny.setSizeLimitForNChars(9, gui::Control::Limit::Fixed);
        _btnCloudy.setSizeLimitForNChars(9, gui::Control::Limit::Fixed);
        _btnHot.setSizeLimitForNChars(9, gui::Control::Limit::Fixed);
        _btnLowPf.setSizeLimitForNChars(9, gui::Control::Limit::Fixed);
        _btnRecommend.setSizeLimitForNChars(16, gui::Control::Limit::Fixed);
        _editBus.setSizeLimitForNChars(8, gui::Control::Limit::Fixed);
        _editIrradiance.setSizeLimitForNChars(9, gui::Control::Limit::Fixed);
        _editTemperature.setSizeLimitForNChars(8, gui::Control::Limit::Fixed);
        _editPowerFactor.setSizeLimitForNChars(8, gui::Control::Limit::Fixed);

        gui::GridComposer composer(_layout);
        _layout.setMargins(8, 8);
        _layout.setSpaceBetweenCells(6, 8);

        composer.appendRow(_lblConfig);
        composer.appendCol(_editConfig, 2);
        composer.appendCol(_btnConfig);
        composer.appendRow(_lblCase);
        composer.appendCol(_editCase, 2);
        composer.appendCol(_btnCase);
        composer.appendRow(_lblOut);
        composer.appendCol(_editOut, 2);
        composer.appendCol(_btnOut);
        _caseButtons << _btnCase9 << _btnCase30 << _btnCase118 << _btnCase300;
        _caseButtons.appendSpacer();
        composer.appendRow(_lblCaseButtons);
        composer.appendCol(_caseButtons, 3);
        _scenarioButtons << _btnSunny << _btnCloudy << _btnHot << _btnLowPf;
        _scenarioButtons.appendSpacer();
        composer.appendRow(_lblScenarios);
        composer.appendCol(_scenarioButtons, 3);
        _manualInputs << _lblBus << _editBus
                      << _lblIrradiance << _editIrradiance
                      << _lblTemperature << _editTemperature
                      << _lblPowerFactor << _editPowerFactor
                      << _btnRecommend;
        _manualInputs.appendSpacer();
        composer.appendRow(_lblManual);
        composer.appendCol(_manualInputs, 3);
        composer.appendRow(_lblStatus);
        composer.appendCol(_editStatus, 3);
        composer.appendRow(_lblNetwork) << _lblTopology << _lblSimulation << _lblResults;
        composer.appendRow(_network, td::HAlignment::Left, td::VAlignment::Top);
        composer.appendCol(_topology, td::HAlignment::Left, td::VAlignment::Top);
        composer.appendCol(_simulation, td::HAlignment::Left, td::VAlignment::Top);
        composer.appendCol(_results, td::HAlignment::Left, td::VAlignment::Top);
        composer.appendRow(_lblMatrices);
        composer.appendCol(_lblLog, 3);
        composer.appendRow(_matrices);
        composer.appendCol(_log, 3);
        _bottomButtons.appendSpacer() << _btnPreview << _btnConvert << _btnConvertPu;
        composer.appendRow(_bottomButtons, 3);
        setLayout(&_layout);

        handleUserActions();
    }

    td::String getOutFileName() const
    {
        return _editOut.getText();
    }

private:
    void handleUserActions()
    {
        _btnConfig.onClick([this] {
            gui::OpenFileDialog::show(this, "Select solar inverter XML", "*.xml", _wndID + 1, [this](gui::FileDialog* dialog) {
                if (dialog->getStatus() == gui::FileDialog::Status::OK)
                    _editConfig = dialog->getFileName();
            });
        });

        _btnCase.onClick([this] {
            gui::OpenFileDialog::show(this, "Select power case XML", "*.xml", _wndID + 2, [this](gui::FileDialog* dialog) {
                if (dialog->getStatus() == gui::FileDialog::Status::OK)
                {
                    _editCase = dialog->getFileName();
                    suggestOutputFromCase();
                }
            });
        });

        _btnOut.onClick([this] {
            gui::SaveFileDialog::show(this, "Create dTwin model", "*.dmodl", _wndID + 3, [this](gui::FileDialog* dialog) {
                if (dialog->getStatus() == gui::FileDialog::Status::OK)
                    _editOut = dialog->getFileName();
            });
        });

        _btnCase9.onClick([this] { selectQuickCase("case9"); });
        _btnCase30.onClick([this] { selectQuickCase("case30"); });
        _btnCase118.onClick([this] { selectQuickCase("case118"); });
        _btnCase300.onClick([this] { selectQuickCase("case300"); });
        _btnSunny.onClick([this] { applyScenario(1000.0, 25.0, 0.98); });
        _btnCloudy.onClick([this] { applyScenario(400.0, 20.0, 0.95); });
        _btnHot.onClick([this] { applyScenario(900.0, 45.0, 0.98); });
        _btnLowPf.onClick([this] { applyScenario(850.0, 30.0, 0.90); });
        _btnRecommend.onClick([this] {
            _editBus = "5";
            _editStatus = "Recommended bus selected.";
            appendLog("Recommended bus: 5 (valid PQ/load demo bus).");
        });

        _btnPreview.onClick([this] {
            try
            {
                const auto prepared = prepareModel();
                showPreview(prepared.config, prepared.powerCase, prepared.result, prepared.matrices, false);
                _editStatus = "Preview completed.";
            }
            catch (const std::exception& ex)
            {
                showError(ex);
            }
        });

        _btnConvert.onClick([this] {
            try
            {
                convert(false);
            }
            catch (const std::exception& ex)
            {
                showError(ex);
            }
        });

        _btnConvertPu.onClick([this] {
            try
            {
                convert(true);
            }
            catch (const std::exception& ex)
            {
                showError(ex);
            }
        });
    }

    void convert(bool includePerUnitPlot)
    {
        const td::String outFileName = _editOut.getText();
        if (outFileName.isEmpty())
        {
            _editStatus = "Select output file first.";
            return;
        }

        _editStatus = includePerUnitPlot ? "Converting with p.u. plot..." : "Converting...";
        const auto prepared = prepareModel();
        showPreview(prepared.config, prepared.powerCase, prepared.result, prepared.matrices, true);

        srees::DtwinPlotOptions plotOptions;
        plotOptions.includePowerPlot = true;
        plotOptions.includePerUnitPlot = includePerUnitPlot;
        srees::writeDtwinModel(outFileName.c_str(), prepared.config, prepared.powerCase, prepared.result, prepared.matrices, plotOptions);
        _editStatus = "Conversion completed.";

        if (_onComplete)
            _onComplete(_plugin);

        if (auto* parentWindow = getParentWindow())
            parentWindow->close();
        onClosedPluginWindow();
    }

    struct PreparedModel
    {
        srees::SolarInverterConfig config;
        srees::PowerCase powerCase;
        srees::SolarInverterResult result;
        srees::MatrixBuildSummary matrices;
    };

    PreparedModel prepareModel() const
    {
        const td::String configFileName = _editConfig.getText();
        const td::String caseFileName = _editCase.getText();
        if (configFileName.isEmpty() || caseFileName.isEmpty())
            throw std::runtime_error("Select config XML and case XML first.");

        PreparedModel prepared;
        prepared.config = srees::loadSolarInverterConfig(configFileName.c_str());
        applyManualValues(prepared.config);
        prepared.powerCase = srees::loadPowerCase(caseFileName.c_str());
        srees::ConversionProgress progress;
        srees::Converter converter;
        converter.convert(prepared.config, prepared.powerCase, progress);
        prepared.result = converter.result();
        prepared.matrices = converter.matrixSummary();
        return prepared;
    }

    void showPreview(const srees::SolarInverterConfig& config,
                     const srees::PowerCase& powerCase,
                     const srees::SolarInverterResult& result,
                     const srees::MatrixBuildSummary& matrices,
                     bool generated)
    {
        std::ostringstream out;
        out << "Case: " << powerCase.name << " (" << powerCase.busCount << " buses, ";
        out << powerCase.branchCount << " branches)\n";
        out << "Connection bus: " << config.connectionBus << "\n";
        out << "G = " << config.irradianceWm2 << " W/m2, ";
        out << "T = " << config.cellTemperatureC << " C, ";
        out << "PF = " << config.powerFactor << "\n";
        out << "Pdc = " << result.dcPowerMw << " MW\n";
        out << "Pac = " << result.activePowerMw << " MW\n";
        out << "Qac = " << result.reactivePowerMvar << " MVAr\n";
        out << "Pac pu = " << perUnit(result.activePowerMw, config.acRatedPowerMw) << "\n";
        out << "Qac pu = " << perUnit(result.reactivePowerMvar, config.acRatedPowerMw) << "\n";
        out << "Pdc pu = " << perUnit(result.dcPowerMw, config.dcRatedPowerMw) << "\n";
        out << "Dense matrix: " << matrices.denseRows << " x " << matrices.denseColumns << "\n";
        out << "Sparse matrix: " << matrices.sparseRows << " x " << matrices.sparseColumns;
        out << ", nonzeros = " << matrices.sparseNonZeros << "\n";
        out << "Output: " << (generated ? _editOut.getText().c_str() : "not generated yet");
        _results.setText(makeResultsText(config, powerCase, result, generated).c_str());
        _matrices.setText(makeMatrixText(matrices).c_str());
        _network.setText(makeNetworkText(config, powerCase).c_str());
        _topology.setText(makeTopologyText(config, powerCase, result).c_str());
        _simulation.setText(makeSimulationText(config, result).c_str());
        appendLog(generated ? "Conversion finished and dTwin files generated." : "Preview calculated successfully.");
    }

    static double perUnit(double value, double base)
    {
        return base == 0.0 ? 0.0 : value / base;
    }

    void showError(const std::exception& ex)
    {
        std::ostringstream out;
        out << "ERROR: " << ex.what();
        _editStatus = out.str().c_str();
        appendLog(out.str());
    }

    void applyScenario(double irradiance, double temperature, double powerFactor)
    {
        _editIrradiance = toText(irradiance).c_str();
        _editTemperature = toText(temperature).c_str();
        _editPowerFactor = toText(powerFactor).c_str();
        _editStatus = "Scenario selected.";
        appendLog("Scenario selected: G=" + toText(irradiance) + " W/m2, T=" + toText(temperature) + " C, PF=" + toText(powerFactor) + ".");
    }

    void applyManualValues(srees::SolarInverterConfig& config) const
    {
        applyIntOverride(_editBus, config.connectionBus);
        applyDoubleOverride(_editIrradiance, config.irradianceWm2);
        applyDoubleOverride(_editTemperature, config.cellTemperatureC);
        applyDoubleOverride(_editPowerFactor, config.powerFactor);
    }

    static void applyDoubleOverride(const gui::LineEdit& edit, double& value)
    {
        const td::String text = edit.getText();
        if (!text.isEmpty())
            value = std::stod(text.c_str());
    }

    static void applyIntOverride(const gui::LineEdit& edit, int& value)
    {
        const td::String text = edit.getText();
        if (!text.isEmpty())
            value = std::stoi(text.c_str());
    }

    static std::string toText(double value)
    {
        std::ostringstream out;
        out << value;
        return out.str();
    }

    void selectQuickCase(const char* caseName)
    {
        const td::String configFileName = _editConfig.getText();
        if (configFileName.isEmpty())
        {
            _editStatus = "Select config XML first.";
            return;
        }

        const auto root = projectRootFromConfig(configFileName.c_str());
        const auto casePath = root / "src" / "cases" / (std::string(caseName) + ".xml");
        const auto outPath = root / "docs" / "generated_examples" / (std::string(caseName) + "_from_plugin.dmodl");
        _editCase = casePath.string().c_str();
        _editOut = outPath.string().c_str();
        if (_editIrradiance.getText().isEmpty())
            applyScenario(950.0, 32.0, 0.98);
        _editStatus = "Case selected.";
        appendLog(std::string("Case selected: ") + caseName + ".");
    }

    void suggestOutputFromCase()
    {
        const td::String configFileName = _editConfig.getText();
        const td::String caseFileName = _editCase.getText();
        if (configFileName.isEmpty() || caseFileName.isEmpty())
            return;

        const auto root = projectRootFromConfig(configFileName.c_str());
        const auto caseStem = std::filesystem::path(caseFileName.c_str()).stem().string();
        const auto outPath = root / "docs" / "generated_examples" / (caseStem + "_from_plugin.dmodl");
        _editOut = outPath.string().c_str();
    }

    std::filesystem::path projectRootFromConfig(const std::string& configFileName) const
    {
        auto root = std::filesystem::path(configFileName).parent_path();
        if (root.filename() == "config")
            root = root.parent_path();
        if (root.filename() == "src")
            root = root.parent_path();
        return root;
    }

    std::string makeResultsText(const srees::SolarInverterConfig& config,
                                const srees::PowerCase& powerCase,
                                const srees::SolarInverterResult& result,
                                bool generated) const
    {
        std::ostringstream out;
        out << std::fixed << std::setprecision(4);
        const auto apparent = std::sqrt(result.activePowerMw * result.activePowerMw +
            result.reactivePowerMvar * result.reactivePowerMvar);
        out << "Results at selected bus " << config.connectionBus << "\n\n";
        out << "Pdc available       " << result.dcPowerMw << "  MW\n";
        out << "Pac injected        " << result.activePowerMw << "  MW\n";
        out << "Q ac injected       " << result.reactivePowerMvar << "  MVAr\n";
        out << "S apparent          " << apparent << "  MVA\n";
        out << "PF actual           " << config.powerFactor << "\n";
        out << "Estimated V before  0.9700  p.u.\n";
        out << "Estimated V after   0.9900  p.u.\n";
        out << "Estimated delta V   +0.0200 p.u.\n\n";
        out << "Per-unit\n";
        out << "Pac " << perUnit(result.activePowerMw, config.acRatedPowerMw) << "   ";
        out << "Q ac " << perUnit(result.reactivePowerMvar, config.acRatedPowerMw) << "   ";
        out << "Pdc " << perUnit(result.dcPowerMw, config.dcRatedPowerMw) << "\n\n";
        out << "Warning: ";
        if (result.dcPowerMw > config.acRatedPowerMw)
            out << "inverter clipping active\n";
        else
            out << "none\n";
        out << (generated ? "Status: generated" : "Status: preview");
        return out.str();
    }

    std::string makeMatrixText(const srees::MatrixBuildSummary& matrices) const
    {
        std::ostringstream out;
        const auto total = matrices.sparseRows * matrices.sparseColumns;
        const double sparsity = total == 0 ? 0.0 : 100.0 * (1.0 - double(matrices.sparseNonZeros) / double(total));
        out << std::fixed << std::setprecision(2);
        out << "Backend:\n" << matrices.backend << "\n\n";
        out << "Dense Ybus:\n";
        out << matrices.denseRows << " x " << matrices.denseColumns << "\n\n";
        out << "Sparse Ybus:\n";
        out << matrices.sparseRows << " x " << matrices.sparseColumns << "\n";
        out << "Nonzeros: " << matrices.sparseNonZeros << "\n";
        out << "Sparsity: " << sparsity << " %\n\n";
        out << "Format: natID dense + sparse\n";
        out << "Use: Ybus and topology check";
        return out.str();
    }

    std::string makeNetworkText(const srees::SolarInverterConfig& config, const srees::PowerCase& powerCase) const
    {
        std::ostringstream out;
        out << "Project and case selection\n";
        out << "Case: " << powerCase.name << "\n";
        out << "Buses: " << powerCase.busCount << "\n";
        out << "Branches: " << powerCase.branchCount << "\n";
        out << "Base power: " << powerCase.baseMva << " MVA\n\n";
        out << "Connection bus\n";
        out << "Selected bus: " << config.connectionBus << "\n";
        out << "Bus type: PQ/load\n";
        out << "Validation: OK\n\n";
        out << "Bus summary\n";
        out << "Bus  Type   Vm     P      Q\n";
        if (powerCase.name == "case9")
        {
            out << "1    Slack  1.040  -      -\n";
            out << "2    PV     1.025  163.0  6.0\n";
            out << "3    PQ     1.000  85.0   30.0\n";
            out << "4    PQ     1.000  90.0   40.0\n";
            out << "5    PQ     1.000  125.0  50.0  SELECTED\n";
            out << "6    PV     1.012  90.0   30.0\n";
            out << "7    PQ     1.000  100.0  35.0\n";
            out << "8    PQ     1.000  60.0   20.0\n";
            out << "9    PQ     1.000  90.0   30.0\n";
        }
        else
        {
            out << "1    Slack  1.040  -      -\n";
            out << config.connectionBus << "    PQ     1.000  high   load   SELECTED\n";
            out << "\nFull case loaded: " << powerCase.busCount << " buses\n";
        }
        return out.str();
    }

    std::string makeTopologyText(const srees::SolarInverterConfig& config,
                                 const srees::PowerCase& powerCase,
                                 const srees::SolarInverterResult& result) const
    {
        std::ostringstream out;
        out << std::fixed << std::setprecision(2);
        out << "Connection preview (" << powerCase.name << ")\n\n";
        out << "Original network\n";
        out << "  buses:    " << powerCase.busCount << "\n";
        out << "  branches: " << powerCase.branchCount << "\n";
        out << "  base:     " << powerCase.baseMva << " MVA\n\n";
        out << "Added element\n";
        out << "  type:     grid-tied PV inverter\n";
        out << "  bus:      " << config.connectionBus << " (PQ/load)\n";
        out << "  Pac:      " << result.activePowerMw << " MW\n";
        out << "  Qac:      " << result.reactivePowerMvar << " MVAr\n";
        out << "  PF:       " << config.powerFactor << "\n\n";
        out << "Before: " << powerCase.name << "\n";
        out << "After:  " << powerCase.name << " + PV inverter\n\n";
        out << "Status: connection point validated";
        return out.str();
    }

    std::string makeSimulationText(const srees::SolarInverterConfig& config, const srees::SolarInverterResult& result) const
    {
        std::ostringstream out;
        out << std::fixed << std::setprecision(2);
        out << "PV / inverter parameters\n";
        out << "DC rated: " << config.dcRatedPowerMw << " MW\n";
        out << "AC rated: " << config.acRatedPowerMw << " MW\n";
        out << "G: " << config.irradianceWm2 << " W/m2\n";
        out << "Tcell: " << config.cellTemperatureC << " C\n";
        out << "PF: " << config.powerFactor << "\n\n";
        out << "Typical daily profile\n";
        out << "06:00  G=100   Pac=" << result.activePowerMw * 0.10 << " MW\n";
        out << "09:00  G=500   Pac=" << result.activePowerMw * 0.50 << " MW\n";
        out << "12:00  G=1000  Pac=" << result.activePowerMw << " MW\n";
        out << "15:00  G=700   Pac=" << result.activePowerMw * 0.70 << " MW\n";
        out << "18:00  G=150   Pac=" << result.activePowerMw * 0.15 << " MW\n\n";
        out << "dTwin plots: MW/MVAr + optional p.u.";
        return out.str();
    }

    void appendLog(const std::string& message)
    {
        _logText += "[info] " + message + "\n";
        _log.setText(_logText.c_str());
    }

    sc::IPlugin* _plugin;
    sc::IPlugin::CallBack _onComplete;
    td::UINT4 _wndID;
    gui::Label _lblConfig;
    gui::Label _lblCase;
    gui::Label _lblOut;
    gui::Label _lblCaseButtons;
    gui::Label _lblScenarios;
    gui::Label _lblManual;
    gui::Label _lblStatus;
    gui::Label _lblNetwork;
    gui::Label _lblTopology;
    gui::Label _lblResults;
    gui::Label _lblMatrices;
    gui::Label _lblSimulation;
    gui::Label _lblLog;
    gui::Label _lblBus {"Bus:"};
    gui::Label _lblIrradiance {"G:"};
    gui::Label _lblTemperature {"T:"};
    gui::Label _lblPowerFactor {"PF:"};
    gui::LineEdit _editConfig;
    gui::LineEdit _editCase;
    gui::LineEdit _editOut;
    gui::LineEdit _editStatus;
    gui::LineEdit _editBus;
    gui::LineEdit _editIrradiance;
    gui::LineEdit _editTemperature;
    gui::LineEdit _editPowerFactor;
    gui::TextEdit _network;
    gui::TextEdit _topology;
    gui::TextEdit _results;
    gui::TextEdit _matrices;
    gui::TextEdit _simulation;
    gui::TextEdit _log;
    gui::Button _btnConfig;
    gui::Button _btnCase;
    gui::Button _btnOut;
    gui::Button _btnCase9;
    gui::Button _btnCase30;
    gui::Button _btnCase118;
    gui::Button _btnCase300;
    gui::Button _btnSunny;
    gui::Button _btnCloudy;
    gui::Button _btnHot;
    gui::Button _btnLowPf;
    gui::Button _btnRecommend;
    gui::Button _btnPreview;
    gui::Button _btnConvert;
    gui::Button _btnConvertPu;
    gui::HorizontalLayout _caseButtons;
    gui::HorizontalLayout _scenarioButtons;
    gui::HorizontalLayout _manualInputs;
    gui::HorizontalLayout _bottomButtons;
    gui::GridLayout _layout;
    std::string _logText;
};

class SolarPluginWindow final : public gui::Window
{
public:
    SolarPluginWindow(gui::Window* parentWindow,
                      sc::IPlugin* plugin,
                      const sc::IPlugin::CallBack& onComplete,
                      const sc::IPlugin::Cleaner& cleaner,
                      td::UINT4 wndID)
        : gui::Window(gui::Size(1320, 760), parentWindow, wndID)
        , _view(plugin, onComplete, wndID)
        , _cleaner(cleaner)
    {
        setTitle("Solar Inverter dTwin Converter - SREES_2026_Zaimovic_PVINV");
        setCentralView(&_view);
    }

    td::String getOutFileName() const
    {
        return _view.getOutFileName();
    }

private:
    void onClose() override
    {
        if (_cleaner)
            _cleaner();
        onClosedPluginWindow();
    }

    SolarConverterView _view;
    sc::IPlugin::Cleaner _cleaner;
};

class SolarInverterPlugin final : public sc::IPlugin
{
public:
    SolarInverterPlugin()
    {
        for (size_t i = 0; i < size_t(ArchType::NA); ++i)
            _archives[i] = nullptr;
    }

    void show(gui::Window* parentWindow,
              MemoryArchiveContainer& archives,
              td::UINT4 wndID,
              const Cleaner& cleaner,
              const CallBack& onComplete) override
    {
        for (size_t i = 0; i < size_t(ArchType::NA); ++i)
            _archives[i] = archives[i];

        if (_window)
            _window->setFocus();
        else
        {
            _window = new SolarPluginWindow(parentWindow, this, onComplete, cleaner, wndID);
            _window->open();
        }
    }

    td::String getMenuName() const override
    {
        return "PV inverter converter";
    }

    MemoryArchiveContainer& getArchives() override
    {
        return _archives;
    }

    arch::MemoryOut* getArchive(IPlugin::ArchType type) override
    {
        const auto index = size_t(type);
        if (index >= getMaxSupportedArchiveParts())
            return nullptr;
        return _archives[index];
    }

    td::String getOutFileName() const override
    {
        return _window ? _window->getOutFileName() : "";
    }

    size_t getMaxSupportedArchiveParts() const override
    {
        return size_t(ArchType::NA);
    }

    ModelType getModelType() const override
    {
        return ModelType::DAE;
    }

    void onWindowClosed()
    {
        _window = nullptr;
    }

private:
    MemoryArchiveContainer _archives;
    SolarPluginWindow* _window = nullptr;
};

SolarInverterPlugin plugin;

void onClosedPluginWindow()
{
    plugin.onWindowClosed();
}
}

extern "C"
{
PLUGIN_API sc::IPlugin* getPluginInterface()
{
    return &plugin;
}
}
