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


#include "Mathematical/Algorithm/Step.hpp"
#include "QCMethod/HF/UHF/UHFSCFEnvironment.hpp"
#include "QCModel/HF/UHF.hpp"


namespace GQCP {


/**
 *  An iteration step that calculates the alpha- and beta- error matrices from the Fock and density matrices (expressed in the scalar/AO basis).
 * 
 *  @tparam _Scalar              The scalar type used to represent the expansion coefficient/elements of the transformation matrix: real or complex.
 */
template <typename _Scalar>
class UHFErrorCalculation:
    public Step<UHFSCFEnvironment<_Scalar>> {

public:
    using Scalar = _Scalar;
    using Environment = UHFSCFEnvironment<Scalar>;


public:
    /*
     *  PUBLIC OVERRIDDEN METHODS
     */

    /**
     *  @return A textual description of this algorithmic step.
     */
    std::string description() const override {
        return "Calculate the current alpha- and beta- error vectors and add them to the environment.";
    }


    /**
     *  Calculate the current alpha- and beta- error vectors and add them to the environment.
     * 
     *  @param environment              The environment that acts as a sort of calculation space.
     */
    void execute(Environment& environment) override {

        // Read F, D and S from the environment.
        const auto& S = environment.S;

        const auto& F = environment.fock_matrices.back();

        const auto& D = environment.density_matrices.back();

        // Calculate the errors and write them to the environment (as a vector).
        const auto error_matrices = QCModel::UHF<Scalar>::calculateError(F, D, S);

        // Transform the error matrices to error vectors.
        const auto error_vectors = SpinResolved<VectorX<Scalar>> {error_matrices.alpha().pairWiseReduced(), error_matrices.beta().pairWiseReduced()};

        environment.error_vectors.push_back(error_vectors);
    }
};


}  // namespace GQCP
