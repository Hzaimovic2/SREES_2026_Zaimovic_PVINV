#include "srees/matrix_adapter.hpp"

#include <dense/Matrix.h>
#include <sparse/IMatrix.h>

namespace srees
{
MatrixBuildSummary buildNetworkMatrices(const PowerCase& powerCase, int connectionBus)
{
    const int n = powerCase.busCount;
    const int nonZeroEstimate = 3 * n;

    dense::DblMatrix denseAdmittance(n, n);
    auto dense = denseAdmittance.getManipulator();

    sparse::IDblMatrix* sparseTopology = sparse::createDblMatrix(
        n,
        n,
        nonZeroEstimate,
        sparse::Symmetry::NonSymmetric);

    int nonZeros = 0;
    for (int i = 0; i < n; ++i)
    {
        dense(i, i) = 2.0;
        sparseTopology->addTriple(i, i, 2.0);
        ++nonZeros;

        if (i + 1 < n)
        {
            dense(i, i + 1) = -1.0;
            dense(i + 1, i) = -1.0;
            sparseTopology->addTriple(i, i + 1, -1.0);
            sparseTopology->addTriple(i + 1, i, -1.0);
            nonZeros += 2;
        }
    }

    const int pvBus = connectionBus - 1;
    dense(pvBus, pvBus) = dense(pvBus, pvBus) + 0.1;
    sparseTopology->addTriple(pvBus, pvBus, 0.1);
    ++nonZeros;

    sparseTopology->release();

    return {"natID dense::DblMatrix + sparse::IDblMatrix", n, n, n, n, nonZeros};
}
}
