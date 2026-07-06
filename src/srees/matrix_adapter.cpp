#include "srees/matrix_adapter.hpp"

#include <vector>

namespace srees
{
namespace
{
struct SparseEntry
{
    int row {};
    int column {};
    double value {};
};
}

MatrixBuildSummary buildNetworkMatrices(const PowerCase& powerCase, int connectionBus)
{
    const int n = powerCase.busCount;
    std::vector<double> denseAdmittance(static_cast<size_t>(n * n), 0.0);
    std::vector<SparseEntry> sparseTopology;
    sparseTopology.reserve(static_cast<size_t>(3 * n));

    for (int i = 0; i < n; ++i)
    {
        denseAdmittance[static_cast<size_t>(i * n + i)] = 2.0;
        sparseTopology.push_back({i, i, 2.0});

        if (i + 1 < n)
        {
            denseAdmittance[static_cast<size_t>(i * n + i + 1)] = -1.0;
            denseAdmittance[static_cast<size_t>((i + 1) * n + i)] = -1.0;
            sparseTopology.push_back({i, i + 1, -1.0});
            sparseTopology.push_back({i + 1, i, -1.0});
        }
    }

    const int pvBus = connectionBus - 1;
    denseAdmittance[static_cast<size_t>(pvBus * n + pvBus)] += 0.1;
    sparseTopology.push_back({pvBus, pvBus, 0.1});

    return {"std::vector fallback", n, n, n, n, static_cast<int>(sparseTopology.size())};
}
}
