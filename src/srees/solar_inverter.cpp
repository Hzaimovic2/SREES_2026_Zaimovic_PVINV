#include "srees/solar_inverter.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace srees
{
SolarInverterResult evaluateSolarInverter(const SolarInverterConfig& config)
{
    if (config.referenceIrradianceWm2 <= 0.0)
        throw std::runtime_error("Reference irradiance must be positive");

    if (config.powerFactor <= 0.0 || config.powerFactor > 1.0)
        throw std::runtime_error("Power factor must be in interval (0, 1]");

    const auto irradianceRatio = config.irradianceWm2 / config.referenceIrradianceWm2;
    const auto temperatureFactor = 1.0 + config.temperatureCoefficientPerC *
        (config.cellTemperatureC - config.referenceTemperatureC);

    SolarInverterResult result;
    result.dcPowerMw = std::max(0.0, config.dcRatedPowerMw * irradianceRatio * temperatureFactor);
    result.activePowerMw = std::min(result.dcPowerMw, config.acRatedPowerMw);

    const auto angle = std::acos(config.powerFactor);
    result.reactivePowerMvar = result.activePowerMw * std::tan(angle);

    return result;
}
}
