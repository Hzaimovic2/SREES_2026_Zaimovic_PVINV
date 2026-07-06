#pragma once

#include "srees/power_case.hpp"
#include "srees/solar_inverter.hpp"
#include "srees/matrix_adapter.hpp"

#include <atomic>
#include <mutex>
#include <string>

namespace srees
{
class ConversionProgress
{
public:
    void update(int percent, std::string message);
    int percent() const;
    std::string message() const;
    bool done() const;

private:
    std::atomic<int> _percent {0};
    mutable std::mutex _messageMutex;
    std::string _message {"waiting"};
};

class Converter
{
public:
    void convert(const SolarInverterConfig& config, const PowerCase& powerCase, ConversionProgress& progress);
    SolarInverterResult result() const;
    MatrixBuildSummary matrixSummary() const;

private:
    mutable std::mutex _resultMutex;
    SolarInverterResult _result {};
    MatrixBuildSummary _matrixSummary {};
};
}
