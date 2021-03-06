// This file is part of GQCG-GQCP.
//
// Copyright (C) 2017-2020  the GQCG developers
//
// GQCG-GQCP is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// GQCG-GQCP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with GQCG-GQCP.  If not, see <http://www.gnu.org/licenses/>.

#pragma once


#include "Mathematical/Algorithm/IterativeAlgorithm.hpp"
#include "Mathematical/Optimization/ConsecutiveIteratesNormConvergence.hpp"
#include "QCMethod/HF/GHF/GHFDensityMatrixCalculation.hpp"
#include "QCMethod/HF/GHF/GHFElectronicEnergyCalculation.hpp"
#include "QCMethod/HF/GHF/GHFErrorCalculation.hpp"
#include "QCMethod/HF/GHF/GHFFockMatrixCalculation.hpp"
#include "QCMethod/HF/GHF/GHFFockMatrixDIIS.hpp"
#include "QCMethod/HF/GHF/GHFFockMatrixDiagonalization.hpp"
#include "QCMethod/HF/GHF/GHFSCFEnvironment.hpp"


namespace GQCP {


/**
 *  A factory class that can construct GHF SCF solvers in an easy way.
 * 
 *  @tparam _Scalar             The scalar type that is used for the coefficient matrix/expansion coefficients: real or complex.
 */
template <typename _Scalar>
class GHFSCFSolver {
public:
    using Scalar = _Scalar;


public:
    /*
     *  PUBLIC STATIC METHODS
     */

    /**
     *  @param threshold                            The threshold that is used in comparing the density matrices.
     *  @param maximum_number_of_iterations         The maximum number of iterations the algorithm may perform.
     * 
     *  @return A plain GHF SCF solver that uses the norm of the difference of two consecutive density matrices as a convergence criterion.
     */
    static IterativeAlgorithm<GHFSCFEnvironment<Scalar>> Plain(const double threshold = 1.0e-08, const size_t maximum_number_of_iterations = 128) {

        // Create the iteration cycle that effectively 'defines' a plain GHF SCF solver.
        StepCollection<GHFSCFEnvironment<Scalar>> plain_ghf_scf_cycle {};
        plain_ghf_scf_cycle
            .add(GHFDensityMatrixCalculation<Scalar>())
            .add(GHFFockMatrixCalculation<Scalar>())
            .add(GHFFockMatrixDiagonalization<Scalar>())
            .add(GHFElectronicEnergyCalculation<Scalar>());

        // Create a convergence criterion on the norm of subsequent density matrices.
        const auto density_matrix_extractor = [](const GHFSCFEnvironment<Scalar>& environment) { return environment.density_matrices; };

        using ConvergenceType = ConsecutiveIteratesNormConvergence<G1DM<Scalar>, GHFSCFEnvironment<Scalar>>;
        const ConvergenceType convergence_criterion {threshold, density_matrix_extractor, "the GHF density matrix in AO basis"};

        return IterativeAlgorithm<GHFSCFEnvironment<Scalar>>(plain_ghf_scf_cycle, convergence_criterion, maximum_number_of_iterations);
    }


    /**
     *  @param minimum_subspace_dimension           The minimum number of Fock matrices that have to be in the subspace before enabling DIIS.
     *  @param maximum_subspace_dimension           The maximum number of Fock matrices that can be handled by DIIS.
     *  @param threshold                            The threshold that is used in comparing the density matrices.
     *  @param maximum_number_of_iterations         The maximum number of iterations the algorithm may perform.
     * 
     *  @return A DIIS GHF SCF solver that uses the norm of the difference of two consecutive density matrices as a convergence criterion.
     */
    static IterativeAlgorithm<GHFSCFEnvironment<Scalar>> DIIS(const size_t minimum_subspace_dimension = 6, const size_t maximum_subspace_dimension = 6, const double threshold = 1.0e-08, const size_t maximum_number_of_iterations = 128) {

        // Create the iteration cycle that effectively 'defines' a DIIS GHF SCF solver.
        StepCollection<GHFSCFEnvironment<Scalar>> diis_ghf_scf_cycle {};
        diis_ghf_scf_cycle
            .add(GHFDensityMatrixCalculation<Scalar>())
            .add(GHFFockMatrixCalculation<Scalar>())
            .add(GHFErrorCalculation<Scalar>())
            .add(GHFFockMatrixDIIS<Scalar>(minimum_subspace_dimension, maximum_subspace_dimension))  // This also calculates the next coefficient matrix.
            .add(GHFElectronicEnergyCalculation<Scalar>());

        // Create a convergence criterion on the norm of subsequent density matrices.
        const std::function<std::deque<G1DM<Scalar>>(const GHFSCFEnvironment<Scalar>&)> density_matrix_extractor = [](const GHFSCFEnvironment<Scalar>& environment) { return environment.density_matrices; };

        using ConvergenceType = ConsecutiveIteratesNormConvergence<G1DM<Scalar>, GHFSCFEnvironment<Scalar>>;
        const ConvergenceType convergence_criterion {threshold, density_matrix_extractor, "the GHF density matrix in AO basis"};

        return IterativeAlgorithm<GHFSCFEnvironment<Scalar>>(diis_ghf_scf_cycle, convergence_criterion, maximum_number_of_iterations);
    }
};


}  // namespace GQCP
