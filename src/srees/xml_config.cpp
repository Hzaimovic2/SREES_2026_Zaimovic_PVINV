#include "srees/xml_config.hpp"

#include <fstream>
#include <regex>
#include <sstream>
#include <stdexcept>

namespace srees
{
std::string readTextFile(const std::string& path)
{
    std::ifstream input(path);
    if (!input)
        throw std::runtime_error("Cannot open file: " + path);

    std::ostringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

std::string readXmlString(const std::string& xml, const std::string& tag, const std::string& attribute, const std::string& defaultValue)
{
    const std::regex pattern("<" + tag + "[^>]*\\s" + attribute + "=\"([^\"]*)\"");
    std::smatch match;
    if (std::regex_search(xml, match, pattern))
        return match[1].str();

    return defaultValue;
}

int readXmlInt(const std::string& xml, const std::string& tag, const std::string& attribute, int defaultValue)
{
    const auto value = readXmlString(xml, tag, attribute, "");
    return value.empty() ? defaultValue : std::stoi(value);
}

double readXmlDouble(const std::string& xml, const std::string& tag, const std::string& attribute, double defaultValue)
{
    const auto value = readXmlString(xml, tag, attribute, "");
    return value.empty() ? defaultValue : std::stod(value);
}

SolarInverterConfig loadSolarInverterConfig(const std::string& path)
{
    const auto xml = readTextFile(path);

    SolarInverterConfig config;
    config.name = readXmlString(xml, "solarInverter", "name", "Solar inverter");
    config.connectionBus = readXmlInt(xml, "connection", "bus", 1);
    config.dcRatedPowerMw = readXmlDouble(xml, "pvArray", "dcRatedPowerMw", 5.0);
    config.acRatedPowerMw = readXmlDouble(xml, "inverter", "acRatedPowerMw", 4.5);
    config.irradianceWm2 = readXmlDouble(xml, "environment", "irradianceWm2", 1000.0);
    config.referenceIrradianceWm2 = readXmlDouble(xml, "environment", "referenceIrradianceWm2", 1000.0);
    config.cellTemperatureC = readXmlDouble(xml, "environment", "cellTemperatureC", 25.0);
    config.referenceTemperatureC = readXmlDouble(xml, "environment", "referenceTemperatureC", 25.0);
    config.temperatureCoefficientPerC = readXmlDouble(xml, "pvArray", "temperatureCoefficientPerC", -0.004);
    config.powerFactor = readXmlDouble(xml, "inverter", "powerFactor", 0.98);

    return config;
}
}
