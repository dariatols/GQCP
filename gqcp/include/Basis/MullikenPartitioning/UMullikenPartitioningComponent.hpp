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


#include "Basis/Transformations/UTransformationComponent.hpp"
#include "Mathematical/Representation/SquareMatrix.hpp"

#include <vector>


namespace GQCP {


/**
 *  One of the components of an unrestricted Mulliken-based partitioning of an AO basis.
 * 
 *  @param _Scalar          The scalar type used to represent an element of the Mulliken projection matrix: real or complex.
 */
template <typename _Scalar>
class UMullikenPartitioningComponent {
public:
    // The scalar type used to represent an element of the Mulliken projection matrix: real or complex.
    using Scalar = _Scalar;


private:
    // A set of indices that correspond to the AOs that are included in the Mulliken-partitioning of the AO basis associated to the component.
    std::vector<size_t> m_indices;

    // The transformation that relates the atomic spin-orbitals to the set of current spin-orbitals, for one of the components.
    UTransformationComponent<Scalar> C;


public:
    /*
     *  MARK: Constructors
     */

    /**
     *  Create a component of an unrestricted Mulliken partitioning from a set of included AO indices.
     * 
     *  @param indices          A set of indices that correspond to the AOs that are included in the Mulliken-partitioning of the AO basis associated to the component.
     *  @param C                The transformation that relates the atomic spin-orbitals to the set of current spin-orbitals, for one of the components.
     */
    UMullikenPartitioningComponent(const std::vector<size_t>& indices, const UTransformationComponent<Scalar>& C) :
        m_indices {indices},
        C {C} {}


    /**
     *  MARK: General information
     */

    /**
     *  @return The set of indices that correspond to the AOs that are included in the Mulliken-partitioning of an AO basis.
     */
    const std::vector<size_t>& indices() const { return this->m_indices; }

    /**
     *  @return The number of orbitals that this Mulliken partitioning is related to.
     */
    size_t numberOfOrbitals() const { return this->C.numberOfOrbitals(); }


    /**
     *  MARK: Partitioning and projecting
     */

    /**
     *  @return The partition matrix 'P_A' related to this Mulliken partitioning.
     */
    SquareMatrix<Scalar> partitionMatrix() const { return SquareMatrix<Scalar>::PartitionMatrix(this->indices(), this->numberOfOrbitals()); }


    /**
     *  @return The Mulliken projection, defined as C^{-1} P_A C, where C is the transformation matrix and P_A is the partition matrix.
     */
    UTransformationComponent<Scalar> projectionMatrix() const { return UTransformationComponent<Scalar> {this->C.matrix().inverse() * this->partitionMatrix() * this->C.matrix()}; }
};


}  // namespace GQCP
