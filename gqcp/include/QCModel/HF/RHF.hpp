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


#include "Basis/SpinorBasis/OrbitalSpace.hpp"
#include "Basis/SpinorBasis/Spin.hpp"
#include "Basis/TransformationMatrix.hpp"
#include "Mathematical/Representation/ImplicitRankFourTensorSlice.hpp"
#include "Mathematical/Representation/QCMatrix.hpp"
#include "Mathematical/Representation/SquareMatrix.hpp"
#include "Operator/SecondQuantized/SQHamiltonian.hpp"
#include "Operator/SecondQuantized/SQOneElectronOperator.hpp"
#include "Processing/RDM/OneRDM.hpp"


namespace GQCP {
namespace QCModel {


/**
 *  The restricted Hartree-Fock wave function model.
 * 
 *  @tparam _Scalar             the type of scalar that is used for the expansion of the spatial orbitals in their underlying scalar basis
 */
template <typename _Scalar>
class RHF {
public:
    using Scalar = _Scalar;


private:
    size_t N_P;  // the number of electron pairs

    VectorX<double> orbital_energies;  // sorted in ascending energies
    TransformationMatrix<Scalar> C;    // the coefficient matrix that expresses every spatial orbital (as a column) in its underlying scalar basis


public:
    /*
     *  CONSTRUCTORS
     */

    /**
     *  The standard member-wise constructor
     * 
     *  @param N_P                  the number of electron pairs
     *  @param C                    the coefficient matrix that expresses every spatial orbital (as a column) in its underlying scalar basis
     *  @param orbital_energies     the RHF MO energies
     */
    RHF(const size_t N_P, const VectorX<double>& orbital_energies, const TransformationMatrix<double>& C) :
        N_P {N_P},
        orbital_energies {orbital_energies},
        C(C) {}


    /**
     *  Default constructor setting everything to zero
     */
    RHF() :
        RHF(0.0, TransformationMatrix<double>::Zero(0, 0), VectorX<double>::Zero(0)) {}


    /*
     *  STATIC PUBLIC METHODS
     */


    /**
     *  @param D                the RHF density matrix in a scalar basis
     *  @param H_core           the core Hamiltonian expressed in the same scalar basis
     *  @param F                the Fock matrix in the same scalar basis
     *
     *  @return the RHF electronic energy
     */
    static double calculateElectronicEnergy(const OneRDM<Scalar>& D, const ScalarSQOneElectronOperator<Scalar>& H_core, const ScalarSQOneElectronOperator<Scalar>& F) {

        // First, calculate the sum of H_core and F (this saves a contraction)
        ScalarSQOneElectronOperator<Scalar> Z = H_core + F;

        // Convert the matrices Z and D to an Eigen::Tensor<double, 2> D_tensor, as contractions are only implemented for Tensors
        Eigen::TensorMap<Eigen::Tensor<const Scalar, 2>> D_tensor {D.data(), D.rows(), D.cols()};
        Eigen::TensorMap<Eigen::Tensor<double, 2>> Z_tensor {Z.parameters().data(), D.rows(), D.cols()};

        // Specify the contraction pair
        // To calculate the electronic energy, we must perform a double contraction
        //      0.5 D(nu mu) Z(mu nu)
        Eigen::array<Eigen::IndexPair<int>, 2> contraction_pair = {Eigen::IndexPair<int>(0, 1), Eigen::IndexPair<int>(1, 0)};

        // Calculate the double contraction (with prefactor 0.5)
        Tensor<Scalar, 0> contraction = 0.5 * D_tensor.contract(Z_tensor, contraction_pair);

        // As the double contraction of two matrices is a scalar (a tensor of rank 0), we should access the value as (0)
        return contraction(0);
    }


    /**
     *  @param F                the Fock matrix expressed in a scalar basis
     *  @param D                the RHF density matrix in the same scalar basis
     *  @param S                the overlap matrix of that scalar basis
     * 
     *  @return the RHF error matrix
     */
    static SquareMatrix<Scalar> calculateError(const QCMatrix<Scalar>& F, const OneRDM<Scalar>& D, const SquareMatrix<Scalar>& S) {
        return F * D * S - S * D * F;
    }


    /**
     *  @param sq_hamiltonian       the Hamiltonian expressed in an orthonormal basis
     *  @param N_P                  the number of electron pairs
     *  @param a                    the first virtual orbital index
     *  @param i                    the first occupied orbital index
     *  @param b                    the second virtual orbital index
     *  @param j                    the second occupied orbital index
     * 
     *  @return an element of the RHF orbital Hessian
     */
    static Scalar calculateOrbitalHessianRRElement(const SQHamiltonian<Scalar>& sq_hamiltonian, const size_t N_P, const size_t a, const size_t i, const size_t b, const size_t j) {

        // Prepare some variables.
        const auto& g = sq_hamiltonian.twoElectron().parameters();
        const auto K = g.dimension();  // the number of spatial orbitals

        const auto orbital_space = RHF<Scalar>::orbitalSpace(K, N_P);


        double value {0.0};

        // Inactive Fock matrix part
        const auto F = sq_hamiltonian.calculateInactiveFockianRestricted(orbital_space).parameters();
        if (i == j) {
            value += F(a, b);
        }

        if (a == b) {
            value -= F(i, j);
        }


        // Two-electron part
        value += 4 * g(a, i, b, j) - g(a, b, i, j) - g(a, j, b, i);

        return 4 * value;
    }


    /**
     *  @param sq_hamiltonian       the Hamiltonian expressed in an orthonormal basis
     *  @param N_P                  the number of electron pairs
     * 
     *  @return the real-real RHF orbital Hessian as a ImplicitRankFourTensorSlice, i.e. an object with a suitable operator() implemented
     */
    static ImplicitRankFourTensorSlice<Scalar> calculateOrbitalHessianRR(const SQHamiltonian<Scalar>& sq_hamiltonian, const size_t N_P) {

        // Create an occupied-virtual orbital space.
        const auto K = sq_hamiltonian.dimension();
        const auto orbital_space = OrbitalSpace::Implicit({{OccupationType::k_occupied, N_P}, {OccupationType::k_virtual, K - N_P}});  // N_P occupied spatial orbitals, K-N_P virtual spatial orbitals

        auto hessian = orbital_space.template initializeRepresentableObjectFor<Scalar>(OccupationType::k_virtual, OccupationType::k_occupied, OccupationType::k_virtual, OccupationType::k_occupied);  // zero-initialize an object suitable for the representation of a virtual-occupied,virtual-occupied object (ai,bj)

        // Loop over all indices (ai,bj) to construct the orbital hessian
        for (const auto& a : orbital_space.indices(OccupationType::k_virtual)) {
            for (const auto& i : orbital_space.indices(OccupationType::k_occupied)) {

                for (const auto& b : orbital_space.indices(OccupationType::k_virtual)) {
                    for (const auto& j : orbital_space.indices(OccupationType::k_occupied)) {
                        hessian(a, i, b, j) = RHF<Scalar>::calculateOrbitalHessianRRElement(sq_hamiltonian, N_P, a, i, b, j);
                    }
                }
            }
        }

        return hessian;
    }


    /**
     *  @param K    the number of spatial orbitals
     *  @param N    the number of electrons
     *
     *  @return the RHF 1-RDM expressed in an orthonormal spinor basis
     */
    static OneRDM<Scalar> calculateOrthonormalBasis1RDM(const size_t K, const size_t N) {

        if (N % 2 != 0) {
            throw std::invalid_argument("QCMethod::RHF::calculateOrthonormalBasis1RDM(const size_t, const size_t): The number of given electrons cannot be odd for RHF.");
        }

        // The 1-RDM for RHF looks like (for K=5, N=6)
        //    2  0  0  0  0
        //    0  2  0  0  0
        //    0  0  2  0  0
        //    0  0  0  0  0
        //    0  0  0  0  0

        OneRDM<double> D_MO = OneRDM<double>::Zero(K, K);
        D_MO.topLeftCorner(N / 2, N / 2) = 2 * SquareMatrix<double>::Identity(N / 2, N / 2);

        return D_MO;
    }


    /**
     *  @param C    the coefficient matrix that expresses every spatial orbital (as a column) in its underlying scalar basis
     *  @param N    the number of electrons
     *
     *  @return the RHF 1-RDM expressed in the underlying scalar basis
     */
    static OneRDM<Scalar> calculateScalarBasis1RDM(const TransformationMatrix<double>& C, const size_t N) {

        const size_t K = C.dimension();
        const auto D_orthonormal = RHF<Scalar>::calculateOrthonormalBasis1RDM(K, N);

        // Transform the 1-RDM in an orthonormal basis to the underlying scalar basis
        return C.conjugate() * D_orthonormal * C.transpose();
    }


    /**
     *  Calculate the RHF Fock matrix F = H_core + G, in which G is a contraction of the density matrix and the two-electron integrals
     *
     *  @param D                    the RHF density matrix in a scalar basis
     *  @param sq_hamiltonian       the Hamiltonian expressed in the same scalar basis
     *
     *  @return the RHF Fock matrix expressed in the scalar basis
     */
    static ScalarSQOneElectronOperator<Scalar> calculateScalarBasisFockMatrix(const OneRDM<Scalar>& D, const SQHamiltonian<Scalar>& sq_hamiltonian) {
        // To perform the contraction, we will first have to convert the MatrixX<double> D to an Eigen::Tensor<const double, 2> D_tensor, as contractions are only implemented for Tensors
        Eigen::TensorMap<Eigen::Tensor<const Scalar, 2>> D_tensor {D.data(), D.rows(), D.cols()};

        // Specify the contraction pairs
        // To calculate G, we must perform two double contractions
        //      1. (mu nu|rho lambda) P(lambda rho)
        Eigen::array<Eigen::IndexPair<int>, 2> direct_contraction_pair = {Eigen::IndexPair<int>(3, 0), Eigen::IndexPair<int>(2, 1)};
        //      2. -0.5 (mu lambda|rho nu) P(lambda rho)
        Eigen::array<Eigen::IndexPair<int>, 2> exchange_contraction_pair = {Eigen::IndexPair<int>(1, 0), Eigen::IndexPair<int>(2, 1)};

        // Calculate both contractions (and incorporate prefactors)
        const auto& g = sq_hamiltonian.twoElectron().parameters();
        Tensor<Scalar, 2> direct_contraction = g.contract(D_tensor, direct_contraction_pair);
        Tensor<Scalar, 2> exchange_contraction = -0.5 * g.contract(D_tensor, exchange_contraction_pair);

        // The previous contractions are Tensor<Scalar, 2> instances. In order to calculate the total G matrix, we will convert them back into MatrixX<double>
        Eigen::Map<Eigen::MatrixXd> G1 {direct_contraction.data(), direct_contraction.dimension(0), direct_contraction.dimension(1)};
        Eigen::Map<Eigen::MatrixXd> G2 {exchange_contraction.data(), exchange_contraction.dimension(0), exchange_contraction.dimension(1)};

        return ScalarSQOneElectronOperator<Scalar> {sq_hamiltonian.core().parameters() + G1 + G2};
    }


    /**
     *  @param N            the number of electrons
     *
     *  @return the (spatial orbital, not spin-orbital) index of the RHF HOMO in an implicit orbital space
     */
    static size_t homoIndex(const size_t N) {

        if (N % 2 != 0) {
            throw std::invalid_argument("QCModel::RHF::homoIndex(const size_t): Can't calculate the RHF HOMO index for an odd number of electrons N.");
        }

        return N / 2 - 1;  // need to subtract 1 because computer indices start at 0
    }


    /**
     *  @param K            the number of spatial orbitals
     *  @param N            the number of electrons
     *
     *  @return the (spatial orbital, not spin-orbital) index of the RHF LUMO in an implicit orbital space
     */
    static size_t lumoIndex(const size_t K, const size_t N) {

        if (N >= 2 * K) {
            throw std::invalid_argument("QCModel::RHF::lumoIndex(const size_t, constsize_t): There is no LUMO for the given number of electrons N and spatial orbitals K");
        }

        return RHF<Scalar>::homoIndex(N) + 1;
    }


    /**
     *  @param K            the number of spatial orbitals
     *  @param N_P          the number of electrons
     * 
     *  @return the implicit (i.e. with ascending and contiguous orbital indices) occupied-virtual orbital space that corresponds to these RHF model parameters
     */
    static OrbitalSpace orbitalSpace(const size_t K, const size_t N_P) {

        return OrbitalSpace::Implicit({{OccupationType::k_occupied, N_P}, {OccupationType::k_virtual, K - N_P}});
    }


    /*
     *  PUBLIC METHODS
     */

    /**
     *  @return the 1-RDM expressed in an orthonormal spinor basis related to these optimal RHF parameters
     */
    OneRDM<Scalar> calculateOrthonormalBasis1RDM() const {

        const auto K = this->numberOfSpatialOrbitals();
        const auto N = 2 * this->numberOfElectronPairs();
        return RHF<Scalar>::calculateOrthonormalBasis1RDM(K, N);
    }


    /**
     *  @return the RHF 1-RDM in the scalar/AO basis related to these optimal RHF parameters
     */
    OneRDM<Scalar> calculateScalarBasis1RDM() const {

        const auto N = 2 * this->numberOfElectronPairs();
        return RHF<Scalar>::calculateScalarBasis1RDM(this->coefficientMatrix(), N);
    }


    /**
     *  @return the coefficient matrix that expresses every spatial orbital (as a column) in its underlying scalar basis
     */
    const TransformationMatrix<Scalar>& coefficientMatrix() const { return this->C; }

    /**
     *  @param N            the number of electrons
     *
     *  @return the (spatial orbital, not spin-orbital) index of the RHF HOMO in an implicit orbital space
     */
    size_t homoIndex() const { return RHF<Scalar>::homoIndex(this->numberOfElectrons()); }

    /**
     *  @param K            the number of spatial orbitals
     *  @param N            the number of electrons
     *
     *  @return the (spatial orbital, not spin-orbital) index of the RHF LUMO in an implicit orbital space
     */
    size_t lumoIndex() const { return RHF<Scalar>::lumoIndex(this->numberOfSpatialOrbitals(), this->numberOfElectrons()); }

    /**
     *  @return the number of electron pairs that these RHF model parameters describe
     */
    size_t numberOfElectronPairs() const { return this->N_P; }

    /**
     *  @return the total number of electrons that these RHF model parameters describe
     */
    size_t numberOfElectrons() const { return 2 * this->numberOfElectronPairs(); }

    /**
     *  @param sigma            the spin of the electrons (alpha or beta)
     * 
     *  @return the number of sigma-electrons that these RHF model parameters describe
     */
    size_t numberOfElectrons(const Spin sigma) const { return this->numberOfElectronPairs(); }

    /**
     *  @return the number of spatial orbitals that these RHF model parameters describe
     */
    size_t numberOfSpatialOrbitals() const { return this->coefficientMatrix().dimension(); }

    /**
     *  @return all the spatial orbital energies
     */
    const VectorX<double>& orbitalEnergies() const { return this->orbital_energies; }

    /**
     *  @param i                the index of the orbital
     * 
     *  @return the i-th orbital energy
     */
    double orbitalEnergy(const size_t i) const { return this->orbital_energies(i); }

    /**
     *  @return the implicit occupied-virtual orbital space that is associated to these RHF model parameters
     */
    OrbitalSpace orbitalSpace() const { return RHF<Scalar>::orbitalSpace(this->numberOfSpatialOrbitals(), this->numberOfElectronPairs()); }

    /**
     *  @return all the spin-orbital energies, with the alpha spin-orbital energies appearing before the beta spin-orbital energies
     */
    VectorX<double> spinOrbitalEnergiesBlocked() const {

        const auto K = this->numberOfSpatialOrbitals();

        GQCP::VectorX<double> total_orbital_energies {2 * K};
        total_orbital_energies.head(K) = this->orbitalEnergies();
        total_orbital_energies.tail(K) = this->orbitalEnergies();

        return total_orbital_energies;
    }


    /**
     *  @return all the spin-orbital energies, with the alpha and beta-spinorbital energies interleaved
     */
    VectorX<double> spinOrbitalEnergiesInterleaved() const {

        const auto K = this->numberOfSpatialOrbitals();

        GQCP::VectorX<double> total_orbital_energies {2 * K};
        for (size_t p = 0; p < K; p++) {
            total_orbital_energies(2 * p) = this->orbitalEnergy(p);
            total_orbital_energies(2 * p + 1) = this->orbitalEnergy(p);
        }

        return total_orbital_energies;
    }
};


}  // namespace QCModel
}  // namespace GQCP
