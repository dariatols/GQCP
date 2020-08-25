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

#include "Processing/DensityMatrices/CIDMCalculators/GeneralDMCalculator.hpp"

#include "Processing/DensityMatrices/CIDMCalculators/FrozenCoreDOCIRDMBuilder.hpp"
#include "Processing/DensityMatrices/CIDMCalculators/SeniorityZeroDMCalculator.hpp"
#include "Processing/DensityMatrices/CIDMCalculators/SpinResolvedDMCalculator.hpp"
#include "Processing/DensityMatrices/CIDMCalculators/SpinResolvedFrozenDMCalculator.hpp"
#include "Processing/DensityMatrices/CIDMCalculators/SpinResolvedSelectedDMCalculator.hpp"


namespace GQCP {


/*
 *  CONSTRUCTORS
 */


/**
 *  Allocate a SpinResolvedDMCalculator
 *
 *  @param onv_basis       the FCI ONV basis
 */
GeneralDMCalculator::GeneralDMCalculator(const SpinResolvedONVBasis& onv_basis) :
    dm_calculator {std::make_shared<SpinResolvedDMCalculator>(onv_basis)} {}


/**
 *  Allocate a SpinResolvedSelectedDMCalculator
 *
 *  @param onv_basis       the 'selected' ONV basis
 */
GeneralDMCalculator::GeneralDMCalculator(const SpinResolvedSelectedONVBasis& onv_basis) :
    dm_calculator {std::make_shared<SpinResolvedSelectedDMCalculator>(onv_basis)} {}


/**
 *  A run-time constructor allocating the appropriate derived DMCalculator
 *
 *  @param onv_basis       the ONV basis on which the DMCalculator should be based
 */
GeneralDMCalculator::GeneralDMCalculator(const BaseONVBasis& onv_basis) {

    switch (onv_basis.type()) {

    case ONVBasisType::SpinResolvedONVBasis: {
        this->dm_calculator = std::make_shared<SpinResolvedDMCalculator>(dynamic_cast<const SpinResolvedONVBasis&>(onv_basis));
        break;
    }

    case ONVBasisType::SpinResolvedSelectedONVBasis: {
        this->dm_calculator = std::make_shared<SpinResolvedSelectedDMCalculator>(dynamic_cast<const SpinResolvedSelectedONVBasis&>(onv_basis));
        break;
    }

    case ONVBasisType::SpinResolvedFrozenONVBasis: {
        this->dm_calculator = std::make_shared<SpinResolvedFrozenDMCalculator>(dynamic_cast<const SpinResolvedFrozenONVBasis&>(onv_basis));
        break;
    }

    default: {
        break;
    }
    }
}


/*
 *  PUBLIC METHODS
 */

/**
 *  @return all 1-DMs if a given coefficient vector is set
 */
SpinResolvedOneDM<double> GeneralDMCalculator::calculate1DMs() const {

    if (this->coefficients.rows() == 0) {
        throw std::logic_error("GeneralDMCalculator::calculate1DMs(): No vector has been set.");
    }

    return dm_calculator->calculate1DMs(this->coefficients);
}


/**
 *  @return all 2-DMs if a given coefficient vector is set
 */
SpinResolvedTwoDM<double> GeneralDMCalculator::calculate2DMs() const {

    if (this->coefficients.rows() == 0) {
        throw std::logic_error("GeneralDMCalculator::calculate2DMs(): No vector has been set.");
    }

    return dm_calculator->calculate2DMs(this->coefficients);
}


/**
 *  @param bra_indices      the indices of the orbitals that should be annihilated on the left (on the bra)
 *  @param ket_indices      the indices of the orbitals that should be annihilated on the right (on the ket)
 *
 *  @return an element of the N-DM, as specified by the given bra and ket indices
 *
 *      calculateElement({0, 1}, {2, 1}) would calculate d^{(2)} (0, 1, 1, 2): the operator string would be a^\dagger_0 a^\dagger_1 a_2 a_1
 */
double GeneralDMCalculator::calculateElement(const std::vector<size_t>& bra_indices, const std::vector<size_t>& ket_indices) const {

    if (this->coefficients.rows() == 0) {
        throw std::logic_error("GeneralDMCalculator::calculateElement(std::vector<size_t>, std::vector<size_t>): No vector has been set.");
    }

    return this->dm_calculator->calculateElement(bra_indices, ket_indices, this->coefficients);
}


}  // namespace GQCP