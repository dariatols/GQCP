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

#include "QCMethod/CI/HamiltonianBuilder/FrozenCoreFCI.hpp"

#include "QCMethod/CI/HamiltonianBuilder/FCI.hpp"


namespace GQCP {


/*
 *  CONSTRUCTORS
 */

/**
 *  @param onv_basis       the spin-resolved frozen ONV basis
 */
FrozenCoreFCI::FrozenCoreFCI(const SpinResolvedFrozenONVBasis& onv_basis) :
    FrozenCoreCI(std::make_shared<FCI>(onv_basis.activeONVBasis()), onv_basis.numberOfFrozenOrbitals()),
    onv_basis {onv_basis} {}


}  // namespace GQCP