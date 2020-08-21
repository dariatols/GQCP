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


#include "Operator/SecondQuantized/SQOneElectronOperator.hpp"
#include "Processing/RDM/OneRDM.hpp"
#include "QCModel/CI/LinearExpansion.hpp"


namespace GQCP {


/**
 *  Calculate the electric polarizability from the linear wave function response
 * 
 *  @param F_p          the electric response force (d^2E/dFdp)
 *  @param response     the linear wave function response
 * 
 *  @return the components of the electric polarizability
 */
Matrix<double, 3, 3> calculateElectricPolarizability(const Matrix<double, Dynamic, 3>& F_p, const Matrix<double, Dynamic, 3>& response);


/**
 *  Calculate the electric polarizability from the linear wave function response and the linear multiplier response
 * 
 *  @param F_p              the electric parameter response force
 *  @param x                the linear wave function response
 *  @param A_lambda         the first part of the electric multiplier response force
 *  @param y                the linear multiplier response
 * 
 *  @return the components of the electric polarizability
 */
Matrix<double, 3, 3> calculateElectricPolarizability(const Matrix<double, Dynamic, 3>& F_p, const Matrix<double, Dynamic, 3>& x, const Matrix<double, Dynamic, 3>& A_lambda, const Matrix<double, Dynamic, 3>& y);


/**
 *  Calculate the Dyson 'amplitudes' (the coefficients of a Dyson orbital) between two wave function expressed in the same spinor basis 
 * 
 *  @param linear_expansion1        a wave function in a spin-unresolved ONV basis  
 *  @param linear_expansion2        a wave function in a spin-unresolved ONV basis containing one fewer electron and the same number of orbitals that is expressed in the same basis
 *
 *  @return a vector with the Dyson orbital amplitudes
 */
VectorX<double> calculateDysonOrbitalCoefficients(const LinearExpansion<SpinResolvedONVBasis>& linear_expansion1, const LinearExpansion<SpinResolvedONVBasis>& linear_expansion2);


}  // namespace GQCP