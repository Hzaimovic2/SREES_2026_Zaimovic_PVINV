#pragma once

#include <string>

namespace srees
{
struct PowerCase
{
    std::string name;
    int busCount {};
    int branchCount {};
    double baseMva {};
};

PowerCase loadPowerCase(const std::string& path);
}
