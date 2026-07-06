#include "srees/power_case.hpp"
#include "srees/xml_config.hpp"

#include <stdexcept>

namespace srees
{
PowerCase loadPowerCase(const std::string& path)
{
    const auto xml = readTextFile(path);

    PowerCase powerCase;
    powerCase.name = readXmlString(xml, "case", "name", "unknown");
    powerCase.busCount = readXmlInt(xml, "buses", "count", 0);
    powerCase.branchCount = readXmlInt(xml, "branches", "count", 0);
    powerCase.baseMva = readXmlDouble(xml, "base", "mva", 100.0);

    if (powerCase.busCount <= 0)
        throw std::runtime_error("Power case must contain at least one bus");

    return powerCase;
}
}
