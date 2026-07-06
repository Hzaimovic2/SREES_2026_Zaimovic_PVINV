#pragma once

#include "srees/power_case.hpp"

#include <string>

namespace srees
{
struct MatrixBuildSummary
{
    std::string backend;
    int denseRows {};
    int denseColumns {};
    int sparseRows {};
    int sparseColumns {};
    int sparseNonZeros {};
};

MatrixBuildSummary buildNetworkMatrices(const PowerCase& powerCase, int connectionBus);
}
