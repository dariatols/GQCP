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


#include "Basis/Transformations/UTransformation.hpp"
#include "Basis/Transformations/UTransformationComponent.hpp"
#include "DensityMatrix/SpinResolved1DM.hpp"
#include "Mathematical/Representation/SquareMatrix.hpp"
#include "Operator/SecondQuantized/SQHamiltonian.hpp"
#include "Operator/SecondQuantized/USQOneElectronOperator.hpp"
#include "QCModel/HF/RHF.hpp"

#include <Eigen/Dense>

#include <deque>


namespace GQCP {


/**
 *  An algorithm environment that can be used with standard UHF SCF solvers.
 * 
 *  We can basically view it as a compile-time type-safe std::map with all possible information that can be encountered in an UHF SCF algorithm.
 * 
 *  @tparam _Scalar             The scalar type that is used for the coefficient matrix/expansion coefficients: real or complex.
 */
template <typename _Scalar>
class UHFSCFEnvironment {
public:
    using Scalar = _Scalar;


public:
    SpinResolved<size_t> N;  // The number of alpha and beta electrons (the number of occupied alpha-spin-orbitals).

    std::deque<double> electronic_energies;

    std::deque<SpinResolved<VectorX<double>>> orbital_energies;  // The alpha and beta MO energies.

    ScalarUSQOneElectronOperator<Scalar> S;  // The overlap operator (of the scalar (AO) basis).

    std::deque<UTransformation<Scalar>> coefficient_matrices;  // The alpha and beta coefficient matrices.

    std::deque<SpinResolved1DM<double>> density_matrices;  // Expressed in the scalar (AO) basis.

    std::deque<ScalarUSQOneElectronOperator<Scalar>> fock_matrices;  // Expressed in the scalar (AO) basis.

    std::deque<SpinResolved<VectorX<Scalar>>> error_vectors;  // Expressed in the scalar (AO) basis, used when doing DIIS calculations: the real error matrices should be converted to column-major error vectors for the DIIS algorithm to be used correctly.

    USQHamiltonian<Scalar> sq_hamiltonian;  // The Hamiltonian expressed in the scalar (AO) basis.


public:
    /*
     *  CONSTRUCTORS
     */

    /**
     *  A constructor that initializes the environment with initial guesses for the alpha and beta coefficient matrices.
     * 
     *  @param N_alpha                  The number of alpha electrons (the number of occupied alpha-spin-orbitals).
     *  @param N_beta                   The number of beta electrons (the number of occupied beta-spin-orbitals).
     *  @param sq_hamiltonian           The Hamiltonian expressed in the scalar (AO) basis.
     *  @param S                        The overlap matrix (of the scalar (AO) basis).
     *  @param C_alpha_initial          The initial coefficient matrix for the alpha spin-orbitals.
     *  @param C_beta_initial           The initial coefficient matrix for the beta spin-orbitals.
     */
    UHFSCFEnvironment(const size_t N_alpha, const size_t N_beta, const USQHamiltonian<Scalar>& sq_hamiltonian, const ScalarUSQOneElectronOperator<Scalar>& S, const UTransformation<Scalar>& C_initial) :
        N {N_alpha, N_beta},
        S {S},
        sq_hamiltonian {sq_hamiltonian},
        coefficient_matrices {C_initial} {}


    /**
     *  A constructor that initializes the environment from converged RHF model parameters.
     * 
     *  @param rhf_parameters           The converged RHF model parameters.
     *  @param sq_hamiltonian           The Hamiltonian expressed in the scalar (AO) basis.
     *  @param S                        The overlap operator (of the scalar (AO) basis).
     */
    UHFSCFEnvironment(const QCModel::RHF<Scalar>& rhf_parameters, const USQHamiltonian<Scalar>& sq_hamiltonian, const ScalarUSQOneElectronOperator<Scalar>& S) :
        UHFSCFEnvironment(rhf_parameters.numberOfElectrons(Spin::alpha), rhf_parameters.numberOfElectrons(Spin::beta),
                          sq_hamiltonian, S,
                          UTransformation<Scalar>::FromEqual(rhf_parameters.expansion().matrix())) {}


    /*
     *  NAMED CONSTRUCTORS
     */

    /**
     *  Initialize an UHF SCF environment with initial coefficient matrices (equal for alpha and beta) that is obtained by diagonalizing the core Hamiltonian matrix.
     * 
     *  @param N_alpha                  The number of alpha electrons (the number of occupied alpha-spin-orbitals).
     *  @param N_beta                   The number of beta electrons (the number of occupied beta-spin-orbitals).
     *  @param sq_hamiltonian           The Hamiltonian expressed in the scalar (AO) basis.
     *  @param S                        The overlap matrix (of the scalar (AO) basis).
     */
    static UHFSCFEnvironment<Scalar> WithCoreGuess(const size_t N_alpha, const size_t N_beta, const USQHamiltonian<Scalar>& sq_hamiltonian, const ScalarUSQOneElectronOperator<Scalar>& S) {

        const auto& H_core = sq_hamiltonian.core();  // In AO basis.

        using MatrixType = Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>;
        Eigen::GeneralizedSelfAdjointEigenSolver<MatrixType> generalized_eigensolver_a {H_core.alpha().parameters(), S.alpha().parameters()};
        Eigen::GeneralizedSelfAdjointEigenSolver<MatrixType> generalized_eigensolver_b {H_core.beta().parameters(), S.beta().parameters()};
        const UTransformationComponent<Scalar> C_initial_a {generalized_eigensolver_a.eigenvectors()};
        const UTransformationComponent<Scalar> C_initial_b {generalized_eigensolver_b.eigenvectors()};
        const UTransformation<Scalar> C_initial {C_initial_a, C_initial_b};

        return UHFSCFEnvironment<Scalar>(N_alpha, N_beta, sq_hamiltonian, S, C_initial);
    }
};


}  // namespace GQCP
