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

#include "Processing/DensityMatrices/CIDMCalculators/SpinUnresolvedDMCalculator.hpp"


namespace GQCP {


/*
 *  CONSTRUCTORS
 */

/**
 *  @param onv_basis                spin-unresolved ONV basis
 */
SpinUnresolvedDMCalculator::SpinUnresolvedDMCalculator(const SpinUnresolvedONVBasis& onv_basis) :
    onv_basis {onv_basis} {}


/*
 *  PUBLIC OVERRIDDEN METHODS
 */


/**
 *  @param bra_indices      the indices of the orbitals that should be annihilated on the left (on the bra)
 *  @param ket_indices      the indices of the orbitals that should be annihilated on the right (on the ket)
 *  @param x                the coefficient vector representing the UnresolvedCI wave function
 *
 *  @return an element of the N-DM, as specified by the given bra and ket indices
 *
 *      calculateElement({0, 1}, {2, 1}) would calculate d^{(2)} (0, 1, 1, 2): the operator string would be a^\dagger_0 a^\dagger_1 a_2 a_1
 */
double SpinUnresolvedDMCalculator::calculateElement(const std::vector<size_t>& bra_indices, const std::vector<size_t>& ket_indices, const VectorX<double>& x) const {
    // The ket indices should be reversed because the annihilators on the ket should be applied from right to left
    std::vector<size_t> ket_indices_reversed = ket_indices;
    std::reverse(ket_indices_reversed.begin(), ket_indices_reversed.end());


    double value = 0.0;
    int sign = 1;
    SpinUnresolvedONVBasis onv_basis = this->onv_basis;  // make a copy because this method is marked const
    size_t dim = onv_basis.dimension();


    SpinUnresolvedONV bra = onv_basis.constructONVFromAddress(0);
    size_t I = 0;
    while (I < dim) {  // loop over all bra addresses

        // Annihilate the bra on the bra indices
        if (!bra.annihilateAll(bra_indices, sign)) {  // if we can't annihilate, the bra doesn't change

            // Go to the beginning of the outer while loop with the next bra
            if (I < dim - 1) {  // prevent the last permutation from occurring
                onv_basis.transformONVToNextPermutation(bra);
                I++;
                sign = 1;
                continue;
            } else {
                break;  // we have to jump out if we have looped over the whole bra dimension
            }
        }


        SpinUnresolvedONV ket = onv_basis.constructONVFromAddress(0);
        size_t J = 0;
        while (J < dim) {  // loop over all ket indices

            // Annihilate the ket on the ket indices
            if (!ket.annihilateAll(ket_indices_reversed, sign)) {  // if we can't annihilate, the ket doesn't change

                // Go to the beginning of this (the inner) while loop with the next bra
                if (J < dim - 1) {  // prevent the last permutation from occurring
                    onv_basis.transformONVToNextPermutation(ket);
                    J++;
                    sign = 1;
                    continue;
                } else {
                    break;  // we have to jump out if we have looped over the whole ket dimension
                }
            }

            if (bra == ket) {
                value += sign * x(I) * x(J);
            }

            // Reset the previous ket annihilations and move to the next ket
            if (J == dim - 1) {  // prevent the last permutation from occurring
                break;           // out of the J-loop
            }
            ket.createAll(ket_indices_reversed);
            onv_basis.transformONVToNextPermutation(ket);
            sign = 1;
            J++;
        }  // while J loop

        // Reset the previous bra annihilations and move to the next bra
        if (I == dim - 1) {  // prevent the last permutation from occurring
            break;           // out of the I-loop
        }
        bra.createAll(bra_indices);
        onv_basis.transformONVToNextPermutation(bra);
        sign = 1;
        I++;
    }  // while I loop

    return value;
}


}  // namespace GQCP