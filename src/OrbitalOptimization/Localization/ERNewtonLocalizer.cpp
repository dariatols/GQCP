// This file is part of GQCG-gqcp.
// 
// Copyright (C) 2017-2019  the GQCG developers
// 
// GQCG-gqcp is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// GQCG-gqcp is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with GQCG-gqcp.  If not, see <http://www.gnu.org/licenses/>.
// 
#include "OrbitalOptimization/Localization/ERNewtonLocalizer.hpp"



namespace GQCP {


/*
 *  CONSTRUCTORS
 */

/**
 *  @param N_P              the number of electron pairs
 *  @param oo_options       the orbital optimization options that should be used for the orbital optimization algorithm
 */
ERNewtonLocalizer::ERNewtonLocalizer(size_t N_P, const OrbitalOptimizationOptions& oo_options) :
    N_P (N_P),
    NewtonOrbitalOptimizer(oo_options)
{}



/*
 *  PUBLIC OVERRIDDEN METHODS
 */

/**
 *  @param ham_par      the current Hamiltonian parameters
 *
 *  @return the current orbital gradient of the Edmiston-Ruedenberg localization index as a matrix
 */
SquareMatrix<double> ERNewtonLocalizer::calculateGradientMatrix(const HamiltonianParameters<double>& ham_par) const {

    SquareMatrix<double> G = SquareMatrix<double>::Zero(this->N_P, this->N_P);

    for (size_t i = 0; i < this->N_P; i++) {
        for (size_t j = 0; j < this->N_P; j++) {
            G(i,j) = this->calculateGradientMatrixElement(ham_par, i,j);
        }
    }

    return G;
}



/**
 *  @param ham_par      the current Hamiltonian parameters
 *
 *  @return the current orbital Hessian of the Edmiston-Ruedenberg localization index as a tensor
 */
SquareRankFourTensor<double> ERNewtonLocalizer::calculateHessianTensor(const HamiltonianParameters<double>& ham_par) const {

    SquareRankFourTensor<double> H (this->N_P);
    H.setZero();

    for (size_t i = 0; i < this->N_P; i++) {
        for (size_t j = 0; j < this->N_P; j++) {
            for (size_t k = 0; k < this->N_P; k++) {
                for (size_t l = 0; l < this->N_P; l++) {
                    H(i,j,k,l) = this->calculateHessianTensorElement(ham_par, i,j,k,l);
                }
            }
        }
    }

    return H;
}


/**
 *  Use gradient and Hessian information to determine a new direction for the 'full' orbital rotation generators kappa. Note that a distinction is made between 'free' generators, i.e. those that are calculated from the gradient and Hessian information and the 'full' generators, which also include the redundant parameters (that can be set to zero). The 'full' generators are used to calculate the total rotation matrix using the matrix exponential
 * 
 *  @param ham_par      the current Hamiltonian parameters
 * 
 *  @return the new full set orbital generators, including the redundant parameters
 */
VectorX<double> ERNewtonLocalizer::calculateNewFullOrbitalGenerators(const HamiltonianParameters<double>& ham_par) const {

    auto kappa_free = this->calculateNewFreeOrbitalGenerators(ham_par);

    // The free orbital rotation generators only correspond to the occupied-occupied rotations, so we should add the occupied-virtual and virtual-virtual generators
    size_t K = ham_par.get_K();
    size_t dim_full = K * (K - 1) / 2;
    VectorX<double> kappa_full = VectorX<double>::Zero(dim_full);

    size_t dim_free = kappa_free.size();
    kappa_full.head(dim_free) = kappa_free;

    std::cout << "kappa_full: " << std::endl << kappa_full << std::endl << std::endl;

    return kappa_full;
}



/*
 *  PUBLIC METHODS
 */

/**
 *  @param ham_par      the current Hamiltonian parameters
 *  @param i            the row of the gradient 'matrix'
 *  @param j            the column of the gradient 'matrix'
 *
 *  @return the element (i,j) of the Edmiston-Ruedenberg localization index gradient
 */
double ERNewtonLocalizer::calculateGradientMatrixElement(const HamiltonianParameters<double>& ham_par, size_t i, size_t j) const {

    auto g = ham_par.get_g();

    return 4 * (g(j,i,i,i) - g(i,j,j,j));
}


/**
 *  @param ham_par      the current Hamiltonian parameters
 *  @param i            the first index of the Hessian 'tensor'
 *  @param j            the second index of the Hessian 'tensor'
 *  @param k            the third index of the Hessian 'tensor'
 *  @param l            the fourth index of the Hessian 'tensor'
 *
 *  @return the element (i,j,k,l) of the Edmiston-Ruedenberg localization index Hessian
 */
double ERNewtonLocalizer::calculateHessianTensorElement(const HamiltonianParameters<double>& ham_par, size_t i, size_t j, size_t k, size_t l) const {

    auto g = ham_par.get_g();

    // KISS-implementation of the Hessian element for the Edmiston-Ruedenberg localization index
    double value = 0.0;
    if (i == k) {
        value += -2*g(j,l,l,l) - 2*g(l,j,j,j) + 8*g(l,i,j,i) + 4*g(l,j,i,i);
    }

    if (j == k) {
        value += 2*g(i,l,l,l) + 2*g(l,i,i,i) - 8*g(l,j,i,j) - 4*g(l,i,j,j);
    }

    if (i == l) {
        value += 2*g(j,k,k,k) + 2*g(k,j,j,j) - 8*g(k,i,j,i) - 4*g(k,j,i,i);
    }

    if (j == l) {
        value += -2*g(i,k,k,k) - 2*g(k,i,i,i) + 8*g(k,j,i,j) + 4*g(k,i,j,j);
    }

    return value;
}



}  // namespace GQCP
