#pragma once

#include <string>

namespace srees
{
struct SolarInverterConfig
{
    std::string name;
    int connectionBus {};
    double dcRatedPowerMw {};
    double acRatedPowerMw {};
    double irradianceWm2 {};
    double referenceIrradianceWm2 {};
    double cellTemperatureC {};
    double referenceTemperatureC {};
    double temperatureCoefficientPerC {};
    double powerFactor {};
};

struct SolarInverterResult
{
    double dcPowerMw {};
    double activePowerMw {};
    double reactivePowerMvar {};
};

SolarInverterResult evaluateSolarInverter(const SolarInverterConfig& config);
}
