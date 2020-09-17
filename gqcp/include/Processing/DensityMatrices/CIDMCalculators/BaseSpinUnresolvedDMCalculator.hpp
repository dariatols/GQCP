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


#include "ONVBasis/BaseONVBasis.hpp"
#include "Processing/DensityMatrices/OneDM.hpp"
#include "Processing/DensityMatrices/TwoDM.hpp"


namespace GQCP {


/**
 *  BaseSpinUnresolvedDMCalculator is an abstract base class whose derived classes are capable of calculating 1- and 2-DM from wave functions in a spin unresolved basis
 */
class BaseSpinUnresolvedDMCalculator {
public:
    // CONSTRUCTOR
    BaseSpinUnresolvedDMCalculator() = default;


    // DESTRUCTOR
    virtual ~BaseSpinUnresolvedDMCalculator() = default;


    // PUBLIC PURE VIRTUAL METHODS

    /**
     *  @param x        the coefficient vector representing the wave function
     *
     *  @return the 1-DM given a coefficient vector
     */
    virtual OneDM<double> calculate1DM(const VectorX<double>& x) const = 0;

    /**
     *  @param x        the coefficient vector representing the wave function
     *
     *  @return the 2-DM given a coefficient vector
     */
    virtual TwoDM<double> calculate2DM(const VectorX<double>& x) const = 0;

    /**
     *  @return the ONV basis that is associated to this DMCalculator
     */
    virtual const BaseONVBasis* onvBasis() const = 0;
};


}  // namespace GQCP
