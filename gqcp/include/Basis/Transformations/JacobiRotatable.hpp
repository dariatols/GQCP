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


#include "Basis/Transformations/JacobiRotation.hpp"


namespace GQCP {


/*
 *  MARK: JacobiRotatableTraits
 */

/**
 *  A type that provides compile-time information related to the abstract interface `JacobiRotatable`.
 * 
 *  @tparam T       The type that should conform to `JacobiRotatable`.
 */
template <typename T>
struct JacobiRotatableTraits {};


/*
 *  MARK: JacobiRotatable
 */

/**
 *  An (abstract) interface for types that may be transformed from one orbital basis to another, using a Jacobi rotation.
 * 
 *  @param T        The type that should be Jacobi-transformable. It is given as a template argument, enabling CRTP.
 */
template <typename T>
class JacobiRotatable {
public:
    // The type of Jacobi rotation for which the basis rotation should be defined.
    using JacobiRotationType = typename JacobiRotatableTraits<T>::JacobiRotationType;


public:
    /*
     *  MARK: Pure virtual methods
     */

    /**
     *  Apply the Jacobi rotation and return the result.
     * 
     *  @param jacobi_rotation          The Jacobi rotation.
     * 
     *  @return The jacobi-transformed object.
     */
    virtual T rotated(const JacobiRotationType& jacobi_rotation) const = 0;


    /*
     *  MARK: Implementations enabled by pure virtual methods
     */

    /**
     *  In-place apply the Jacobi rotation.
     * 
     *  @param jacobi_rotation          The Jacobi rotation.
     */
    void rotate(const JacobiRotationType& jacobi_rotation) {
        static_cast<T&>(*this) = this->rotated(jacobi_rotation);
    }
};


}  // namespace GQCP