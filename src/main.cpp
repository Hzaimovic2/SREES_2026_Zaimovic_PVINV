#include "srees/converter.hpp"
#include "srees/dtwin_model_writer.hpp"
#include "srees/power_case.hpp"
#include "srees/solar_inverter.hpp"
#include "srees/xml_config.hpp"

#include <chrono>
#include <exception>
#include <iostream>
#include <thread>

int main(int argc, char* argv[])
{
    const std::string configPath = argc > 1 ? argv[1] : "config/solar_inverter.xml";
    const std::string casePath = argc > 2 ? argv[2] : "cases/case9.xml";
    const std::string outModelPath = argc > 3 ? argv[3] : "";

    try
    {
        const auto config = srees::loadSolarInverterConfig(configPath);
        const auto powerCase = srees::loadPowerCase(casePath);

        srees::ConversionProgress progress;
        srees::Converter converter;

        auto conversion = std::thread([&]() {
            converter.convert(config, powerCase, progress);
        });

        auto progressReader = std::thread([&]() {
            while (!progress.done())
            {
                std::cout << "\rProgress: " << progress.percent() << "% " << progress.message() << std::flush;
                std::this_thread::sleep_for(std::chrono::milliseconds(120));
            }
            std::cout << "\rProgress: 100% conversion completed" << std::endl;
        });

        conversion.join();
        progressReader.join();

        const auto result = converter.result();
        const auto matrixSummary = converter.matrixSummary();
        std::cout << "Case: " << powerCase.name << " (" << powerCase.busCount << " buses)" << std::endl;
        std::cout << "PV AC power: " << result.activePowerMw << " MW" << std::endl;
        std::cout << "PV reactive power: " << result.reactivePowerMvar << " MVAr" << std::endl;
        std::cout << "Connection bus: " << config.connectionBus << std::endl;
        std::cout << "Matrix backend: " << matrixSummary.backend << std::endl;
        std::cout << "Dense matrix: " << matrixSummary.denseRows << "x" << matrixSummary.denseColumns << std::endl;
        std::cout << "Sparse matrix: " << matrixSummary.sparseRows << "x" << matrixSummary.sparseColumns
                  << ", nonzeros=" << matrixSummary.sparseNonZeros << std::endl;

        if (!outModelPath.empty())
        {
            srees::writeDtwinModel(outModelPath, config, powerCase, result, matrixSummary);
            std::cout << "dTwin model written to: " << outModelPath << std::endl;
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
