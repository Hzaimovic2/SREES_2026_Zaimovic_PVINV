#pragma once

#include "srees/solar_inverter.hpp"

#include <string>

namespace srees
{
std::string readTextFile(const std::string& path);
std::string readXmlString(const std::string& xml, const std::string& tag, const std::string& attribute, const std::string& defaultValue);
int readXmlInt(const std::string& xml, const std::string& tag, const std::string& attribute, int defaultValue);
double readXmlDouble(const std::string& xml, const std::string& tag, const std::string& attribute, double defaultValue);

SolarInverterConfig loadSolarInverterConfig(const std::string& path);
}
