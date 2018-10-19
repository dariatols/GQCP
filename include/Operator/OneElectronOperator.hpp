// This file is part of GQCG-gqcp.
// 
// Copyright (C) 2017-2018  the GQCG developers
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
#ifndef GQCP_ONEELECTRONOPERATOR_HPP
#define GQCP_ONEELECTRONOPERATOR_HPP


#include <Eigen/Dense>

#include "BaseOperator.hpp"


namespace GQCP {



/**
 *  A class that holds the matrix representation of a one-electron operator in an orbital basis
 */
class OneElectronOperator : public BaseOperator {
private:
    Eigen::MatrixXd matrix;  // the matrix representation of the one-electron operator


public:
    // CONSTRUCTORS
    /**
     *  Constructor based on a given @param matrix
     */
    explicit OneElectronOperator(const Eigen::MatrixXd& matrix);


    // GETTERS
    Eigen::MatrixXd get_matrix_representation() const { return this->matrix; }
    double get(size_t p, size_t q) { return this->matrix(p,q); }

    
    // OPERATORS
    /**
     *  @return the sum of two OneElectronOperators, i.e. a OneElectronOperator whose matrix representation is the sum
     *  of the two matrix representations of the given OneElectronOperators
     */
    GQCP::OneElectronOperator operator+(const GQCP::OneElectronOperator& other);


    // PUBLIC METHODS
    /**
     *  Transform the matrix representation of a one-electron operator using the transformation matrix @param T
     *
     *  Note that the transformation matrix @param T is used as
     *      b' = b T ,
     *  in which the basis functions are collected as elements of a row vector b
     */
    void transform(const Eigen::MatrixXd& T) override;

    /**
     *  Rotate the matrix representation of a one-electron operator using a unitary rotation matrix @param U
     *
     *  Note that the rotation matrix @param U is used as
     *      b' = b U ,
     *  in which the basis functions are collected as elements of a row vector b.
     */
    void rotate(const Eigen::MatrixXd& U) override;

    /**
     *  Rotate the matrix representation of a one-electron operator using the unitary Jacobi rotation matrix U constructed from the @param jacobi_rotation_parameters
     *
     *  Note that
     *      - the rotation matrix @param U is used as
     *          b' = b U ,
     *        in which the basis functions are collected as elements of a row vector b.
     *      - we use the (cos, sin, -sin, cos) definition for the Jacobi rotation matrix
     */
    void rotate(const GQCP::JacobiRotationParameters& jacobi_rotation_parameters) override;


    // FRIEND CLASSES
    friend class HamiltonianParameters;
};



}  // namespace GQCP



#endif  // GQCP_ONEELECTRONOPERATOR_HPP
