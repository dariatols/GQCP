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


#include "ONVBasis/SpinResolvedONVBasis.hpp"
#include "ONVBasis/SpinResolvedSelectedONVBasis.hpp"
#include "ONVBasis/SpinUnresolvedONVBasis.hpp"
#include "Processing/RDM/BaseRDMBuilder.hpp"
#include "QCModel/CI/LinearExpansion.hpp"

#include <boost/range/adaptor/sliced.hpp>
#include <boost/range/adaptor/strided.hpp>
#include <boost/range/algorithm_ext/push_back.hpp>

#include <memory>


namespace GQCP {


/**
 *  A wrapper around the derived RDMBuilders that provides the functionality of the appropriate derived RDMBuilder for a given ONV basis at compile- or runtime.
 */
class RDMCalculator {
private:
    std::shared_ptr<BaseRDMBuilder> rdm_builder;
    VectorX<double> coefficients;

public:
    // CONSTRUCTOR

    /**
     *  The default constructor.
     */
    RDMCalculator() = default;

    /**
     *  Allocate an FCIRDMBuilder
     *
     *  @param onv_basis       the FCI ONV basis
     */
    explicit RDMCalculator(const SpinResolvedONVBasis& onv_basis);

    /**
     *  Allocate a SelectedRDMBuilder
     *
     *  @param onv_basis       the 'selected' ONV basis
     */
    explicit RDMCalculator(const SpinResolvedSelectedONVBasis& onv_basis);

    /**
     *  A run-time constructor allocating the appropriate derived RDMBuilder
     *
     *  @param onv_basis       the ONV basis on which the RDMBuilder should be based
     */
    explicit RDMCalculator(const BaseONVBasis& onv_basis);

    /**
     *  A run-time constructor allocating the appropriate derived RDMBuilder and coefficient vector
     *
     *  @param linear_expansion       the linear expansion in a certain ONV basis
     */
    template <typename ONVBasis>
    explicit RDMCalculator(const LinearExpansion<ONVBasis>& linear_expansion) :
        RDMCalculator(linear_expansion.onvBasis()) {

        this->setCoefficients(linear_expansion.coefficients());
    }


    // OPERATORS

    /**
     *  @param indices_pack      the indices that specify the element of the N-RDM that has to be calculated
     */
    template <typename... size_ts>
    double operator()(size_ts... indices_pack) const {

        // Assume the user has given size_ts
        std::vector<size_t> indices {static_cast<size_t>(indices_pack)...};  // convert the pack to a vector so we can easily traverse


        if ((indices.size() == 0)) {
            return 1.0;  // assume the wave function is normalized
        }

        if ((indices.size() % 2) != 0) {
            throw std::invalid_argument("There must be an even number of indices as arguments.");
        }


        // Split the vector in ket (even) and bra (odd) indices
        std::vector<size_t> bra_indices;  // even
        boost::push_back(bra_indices, indices | boost::adaptors::strided(2));

        std::vector<size_t> ket_indices;  // odd
        boost::push_back(ket_indices, indices | boost::adaptors::sliced(1, indices.size()) | boost::adaptors::strided(2));
        std::reverse(ket_indices.begin(), ket_indices.end());


        return this->calculateElement(bra_indices, ket_indices);
    }


    // PUBLIC METHODS

    /**
     *  @return all 1-RDMs if a given coefficient vector is set
     */
    OneRDMs<double> calculate1RDMs() const;

    /**
     *  @return all 2-RDMs if a given coefficient vector is set
     */
    TwoRDMs<double> calculate2RDMs() const;

    /**
     *  @param bra_indices      the indices of the orbitals that should be annihilated on the left (on the bra)
     *  @param ket_indices      the indices of the orbitals that should be annihilated on the right (on the ket)
     *
     *  @return an element of the N-RDM, as specified by the given bra and ket indices
     *
     *      calculateElement({0, 1}, {2, 1}) would calculate d^{(2)} (0, 1, 1, 2): the operator string would be a^\dagger_0 a^\dagger_1 a_2 a_1
     */
    double calculateElement(const std::vector<size_t>& bra_indices, const std::vector<size_t>& ket_indices) const;

    /**
     *  Replace this instance's coefficients with the given coefficients.
     * 
     *  @param coefficients                 the new coefficients for this instance
     */
    void setCoefficients(const VectorX<double>& coefficients) { this->coefficients = coefficients; };

};


}  // namespace GQCP