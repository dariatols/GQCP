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

#define BOOST_TEST_MODULE "USQOneElectronOperator"

#include <boost/test/unit_test.hpp>

#include "Mathematical/Functions/CartesianGTO.hpp"
#include "Operator/SecondQuantized/USQOneElectronOperator.hpp"
#include "Utilities/miscellaneous.hpp"

#include <boost/math/constants/constants.hpp>


/**
 *  Check the construction of one-electron operators from matrices.
 */
BOOST_AUTO_TEST_CASE(USQOneElectronOperator_constructor) {

    // Check a correct constructor.
    const auto square_matrix = GQCP::SquareMatrix<double>::Zero(4);
    const GQCP::ScalarUSQOneElectronOperator<double> O {square_matrix, square_matrix};


    // Check a faulty constructor.
    // Either both the alpha and beta matrix dimensions are wrong, or only one of them is. All cases are checked.
    const GQCP::MatrixX<double> matrix = GQCP::MatrixX<double>::Zero(3, 4);
    BOOST_CHECK_THROW(GQCP::ScalarUSQOneElectronOperator<double> O_2(matrix, matrix), std::invalid_argument);
    BOOST_CHECK_THROW(GQCP::ScalarUSQOneElectronOperator<double> O_3(matrix, square_matrix), std::invalid_argument);
    BOOST_CHECK_THROW(GQCP::ScalarUSQOneElectronOperator<double> O_4(square_matrix, matrix), std::invalid_argument);
}


/**
 *  Check if the `Zero` named constructor actually sets its parameters to zeros.
 */
BOOST_AUTO_TEST_CASE(Zero) {

    const size_t dim = 2;
    const auto one_op = GQCP::ScalarUSQOneElectronOperator<double>::Zero(2);  // Should initialize to zeros.

    BOOST_CHECK_EQUAL(one_op.numberOfOrbitals(GQCP::Spin::alpha), dim);
    BOOST_CHECK_EQUAL(one_op.numberOfOrbitals(GQCP::Spin::beta), dim);
    BOOST_CHECK(one_op.alpha().parameters().isZero(1.0e-08));
    BOOST_CHECK(one_op.beta().parameters().isZero(1.0e-08));
}


/**
 *  Check if the addition of `USQOneElectronOperator`s works as expected.
 */
BOOST_AUTO_TEST_CASE(USQOneElectronOperator_addition) {

    const size_t dim = 2;

    // Initialize two test matrices and convert them into operators
    GQCP::SquareMatrix<double> M1 {dim};
    // clang-format off
    M1 << 1.0, 2.0,
          3.0, 4.0;
    // clang-format on
    const GQCP::ScalarUSQOneElectronOperator<double> op1 {M1, M1};

    GQCP::SquareMatrix<double> M2 {dim};
    // clang-format off
    M2 << 5.0, 6.0,
          7.0, 8.0;
    // clang-format on
    const GQCP::ScalarUSQOneElectronOperator<double> op2 {M2, M2};


    // Initialize the reference and check the result
    GQCP::SquareMatrix<double> M_sum_ref {dim};
    // clang-format off
    M_sum_ref <<  6.0,  8.0,
                 10.0, 12.0;
    // clang-format on

    const auto op_sum = op1 + op2;
    BOOST_CHECK(op_sum.alpha().parameters().isApprox(M_sum_ref, 1.0e-08));
    BOOST_CHECK(op_sum.beta().parameters().isApprox(M_sum_ref, 1.0e-08));
}


/**
 *  Check if the subtraction of `USQOneElectronOperator`s works as expected.
 */
BOOST_AUTO_TEST_CASE(USQOneElectronOperator_subtraction) {

    const size_t dim = 2;

    // Initialize two test matrices and convert them into operators
    GQCP::SquareMatrix<double> M1 {dim};
    // clang-format off
    M1 << 1.0, 2.0,
          3.0, 4.0;
    // clang-format on
    const GQCP::ScalarUSQOneElectronOperator<double> op1 {M1, M1};

    GQCP::SquareMatrix<double> M2 {dim};
    // clang-format off
    M2 << 5.0, 6.0,
          7.0, 8.0;
    // clang-format on
    const GQCP::ScalarUSQOneElectronOperator<double> op2 {M2, M2};


    // Initialize the reference and check the result
    GQCP::SquareMatrix<double> M_diff_ref {dim};
    // clang-format off
    M_diff_ref << 4.0, 4.0,
                  4.0, 4.0;
    // clang-format on

    const auto op_diff = op2 - op1;
    BOOST_CHECK(op_diff.alpha().parameters().isApprox(M_diff_ref, 1.0e-08));
    BOOST_CHECK(op_diff.beta().parameters().isApprox(M_diff_ref, 1.0e-08));
}


/**
 *  Check if the scalar product with an `USQOneElectronOperator` works as expected.
 */
BOOST_AUTO_TEST_CASE(USQOneElectronOperator_scalar_product) {

    const size_t dim = 2;
    const double scalar = 2.0;

    // Initialize a test matrix and convert it into an operator
    GQCP::SquareMatrix<double> M1 {dim};
    // clang-format off
    M1 << 1.0, 2.0,
          3.0, 4.0;
    // clang-format on
    const GQCP::ScalarUSQOneElectronOperator<double> op1 {M1, M1};

    // Initialize the reference and check the result
    GQCP::SquareMatrix<double> M_ref {dim};
    // clang-format off
    M_ref << 2.0, 4.0,
             6.0, 8.0;
    // clang-format on

    const auto op_result = scalar * op1;
    BOOST_CHECK(op_result.alpha().parameters().isApprox(M_ref, 1.0e-08));
    BOOST_CHECK(op_result.beta().parameters().isApprox(M_ref, 1.0e-08));
}


/**
 *  Check if the negation of an `USQOneElectronOperator` works as expected.
 */
BOOST_AUTO_TEST_CASE(USQOneElectronOperator_negate) {

    const size_t dim = 2;

    // Initialize a test matrix and convert it into an operator
    GQCP::SquareMatrix<double> M1 {dim};
    // clang-format off
    M1 << 1.0, 2.0,
          3.0, 4.0;
    // clang-format on
    const GQCP::ScalarUSQOneElectronOperator<double> op1 {M1, M1};

    // Initialize the reference and check the result
    GQCP::SquareMatrix<double> M_ref {dim};
    // clang-format off
    M_ref << -1.0, -2.0,
             -3.0, -4.0;
    // clang-format on

    const auto op_result = -op1;
    BOOST_CHECK(op_result.alpha().parameters().isApprox(M_ref, 1.0e-08));
    BOOST_CHECK(op_result.beta().parameters().isApprox(M_ref, 1.0e-08));
}


/**
 *  Check if calculateExpectationValue throws when necessary.
 */
BOOST_AUTO_TEST_CASE(calculateExpectationValue_throw) {

    const size_t dim = 2;

    // Initialize test operator.
    GQCP::SquareMatrix<double> M1 {dim};
    // clang-format off
    M1 << 0.0, 0.0,
          0.0, 0.0;
    // clang-format on
    const GQCP::ScalarUSQOneElectronOperator<double> h {M1, M1};

    // Initialize test DMs and check if calculating expectation values throws when expected.
    const GQCP::SpinResolved1DM<double> D_valid {GQCP::OneDM<double>::Zero(dim), GQCP::OneDM<double>::Zero(dim)};
    const GQCP::SpinResolved1DM<double> D_invalid_alpha {GQCP::OneDM<double>::Zero(dim + 1), GQCP::OneDM<double>::Zero(dim)};
    const GQCP::SpinResolved1DM<double> D_invalid_beta {GQCP::OneDM<double>::Zero(dim), GQCP::OneDM<double>::Zero(dim + 1)};

    BOOST_CHECK_THROW(h.calculateExpectationValue(D_invalid_alpha), std::invalid_argument);
    BOOST_CHECK_THROW(h.calculateExpectationValue(D_invalid_beta), std::invalid_argument);

    BOOST_CHECK_NO_THROW(h.calculateExpectationValue(D_valid));
}


/**
 *  Check whether or not calculateExpectationValue shows the correct behaviour.
 */
BOOST_AUTO_TEST_CASE(calculateExpectationValue_behaviour) {

    const size_t dim = 2;

    // Initialize a test operator.
    GQCP::SquareMatrix<double> M1 {dim};
    // clang-format off
    M1 << 1.0, 2.0,
          3.0, 4.0;
    // clang-format on
    const GQCP::ScalarUSQOneElectronOperator<double> op {M1, M1};

    // Initialize an alpha and beta density matrix, each one is chosen as a Hermitian matrix.
    GQCP::SquareMatrix<double> D_alpha {dim};
    // clang-format off
    D_alpha << 0.0, 1.0,
               1.0, 0.0;
    // clang-format on

    GQCP::SquareMatrix<double> D_beta {dim};
    // clang-format off
    D_beta << 1.0,  0.0,
              0.0, -1.0;
    // clang-format on
    const GQCP::SpinResolved1DM<double> D {D_alpha, D_beta};


    // Initialize a reference value and check the result of the calculation.
    const double reference_expectation_value = 2.0;

    const double expectation_value = op.calculateExpectationValue(D);  // A scalar-StorageArray can be implicitly converted to its underlying scalar value.
    BOOST_CHECK(std::abs(expectation_value - reference_expectation_value) < 1.0e-08);
}


/**
 *  Check if a trivial rotation works as expected.
 */
BOOST_AUTO_TEST_CASE(rotate_with_unitary_transformation_matrix) {

    const size_t dim = 2;

    // Initialize a test matrix and convert it into an operator
    GQCP::SquareMatrix<double> M1 {dim};
    // clang-format off
    M1 << 1.0, 2.0,
          3.0, 4.0;
    // clang-format on
    GQCP::ScalarUSQOneElectronOperator<double> op {M1, M1};

    // Initialize a unitary transformation matrix
    const auto U = GQCP::UTransformationMatrix<double>::Identity(dim);

    op.rotate(U);
    BOOST_CHECK(op.alpha().parameters().isApprox(M1, 1.0e-08));
    BOOST_CHECK(op.beta().parameters().isApprox(M1, 1.0e-08));
}


/**
 *  Check whether or not the transformation with a transformation matrix method works as expected.
 */
BOOST_AUTO_TEST_CASE(transform_with_transformation_matrix) {

    const size_t dim = 2;

    // Initialize a test matrix and convert it into an operator
    GQCP::SquareMatrix<double> M1 {dim};
    // clang-format off
    M1 << 1.0, 2.0,
          3.0, 4.0;
    // clang-format on
    GQCP::ScalarUSQOneElectronOperator<double> op {M1, M1};

    // Initialize a transformation matrix
    GQCP::UTransformationMatrixComponent<double> T_component {dim};
    // clang-format off
    T_component << 2.0, 3.0,
                   4.0, 5.0;
    // clang-format on
    const auto T = GQCP::UTransformationMatrix<double>::FromEqual(T_component);

    // Initialize a reference matrix
    GQCP::SquareMatrix<double> ref {dim};
    // clang-format off
    ref << 108.0, 142.0,
           140.0, 184.0;
    // clang-format on

    op.transform(T);
    BOOST_CHECK(op.alpha().parameters().isApprox(ref, 1.0e-08));
    BOOST_CHECK(op.beta().parameters().isApprox(ref, 1.0e-08));
}


/**
 * Check whether or not the rotate with Jacobi rotation method works as expected
 */
// BOOST_AUTO_TEST_CASE(transform_with_jacobi_matrix) {

//     const size_t dim = 4;

//     // Initialize a test matrix and convert it into an operator
//     GQCP::SquareMatrix<double> M1 {dim};
//     // clang-format off
//     M1 <<  1.0,  2.0,  3.0,  4.0,
//            5.0,  6.0,  7.0,  8.0,
//            9.0, 10.0, 11.0, 12.0,
//           13.0, 14.0, 15.0, 16.0;
//     // clang-format on
//     GQCP::ScalarUSQOneElectronOperator<double> op {M1, M1};

//     // Initialize a transformation matrix
//     GQCP::JacobiRotation J {2, 1, (boost::math::constants::pi<double>() / 2)};

//     // Initialize a reference matrix
//     GQCP::SquareMatrix<double> ref {dim};
//     // clang-format off
//     ref <<  1.0,  3.0,  -2.0,  4.0,
//             9.0, 11.0, -10.0, 12.0,
//            -5.0, -7.0,   6.0, -8.0,
//            13.0, 15.0, -14.0, 16.0;
//     // clang-format on

//     op.rotate(J);
//     BOOST_CHECK(op.parameters(GQCP::Spin::alpha).isApprox(ref, 1.0e-08));
//     BOOST_CHECK(op.parameters(GQCP::Spin::beta).isApprox(ref, 1.0e-08));
// }
