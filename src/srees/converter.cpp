#include "srees/converter.hpp"

#include <chrono>
#include <stdexcept>
#include <thread>

namespace srees
{
void ConversionProgress::update(int percent, std::string message)
{
    _percent.store(percent);
    {
        const std::lock_guard<std::mutex> lock(_messageMutex);
        _message = std::move(message);
    }
}

int ConversionProgress::percent() const
{
    return _percent.load();
}

std::string ConversionProgress::message() const
{
    const std::lock_guard<std::mutex> lock(_messageMutex);
    return _message;
}

bool ConversionProgress::done() const
{
    return percent() >= 100;
}

void Converter::convert(const SolarInverterConfig& config, const PowerCase& powerCase, ConversionProgress& progress)
{
    if (config.connectionBus < 1 || config.connectionBus > powerCase.busCount)
        throw std::runtime_error("Configured connection bus does not exist in selected case");

    progress.update(10, "loading network case");
    std::this_thread::sleep_for(std::chrono::milliseconds(150));

    progress.update(35, "building dense admittance data");
    const auto matrixSummary = buildNetworkMatrices(powerCase, config.connectionBus);

    progress.update(70, "dense and sparse matrices prepared");
    const auto converted = evaluateSolarInverter(config);

    {
        const std::lock_guard<std::mutex> lock(_resultMutex);
        _result = converted;
        _matrixSummary = matrixSummary;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    progress.update(100, "conversion completed");
}

SolarInverterResult Converter::result() const
{
    const std::lock_guard<std::mutex> lock(_resultMutex);
    return _result;
}

MatrixBuildSummary Converter::matrixSummary() const
{
    const std::lock_guard<std::mutex> lock(_resultMutex);
    return _matrixSummary;
}
}
