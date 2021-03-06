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


#include "Basis/Transformations/GTransformation.hpp"
#include "DensityMatrix/G1DM.hpp"
#include "DensityMatrix/G2DM.hpp"
#include "Mathematical/Representation/DenseVectorizer.hpp"
#include "Operator/SecondQuantized/GSQOneElectronOperator.hpp"
#include "Operator/SecondQuantized/PureUSQTwoElectronOperatorComponent.hpp"
#include "Operator/SecondQuantized/SimpleSQTwoElectronOperator.hpp"
#include "QuantumChemical/spinor_tags.hpp"


namespace GQCP {


/**
 *  A general(ized) two-electron operator, which is suited for expressing spin-dependent two-electron operators.
 * 
 *  @tparam _Scalar                 The scalar type used for a single parameter/matrix element: real or complex.
 *  @tparam _Vectorizer             The type of the vectorizer that relates a one-dimensional storage of tensors to the tensor structure of two-electron operators. This distinction is carried over from SimpleSQOneElectronOperator.
 */
template <typename _Scalar, typename _Vectorizer>
class GSQTwoElectronOperator:
    public SimpleSQTwoElectronOperator<_Scalar, _Vectorizer, GSQTwoElectronOperator<_Scalar, _Vectorizer>> {
public:
    // The scalar type used for a single parameter/matrix element: real or complex.
    using Scalar = _Scalar;

    //The type of the vectorizer that relates a one-dimensional storage of tensors to the tensor structure of two-electron operators. This distinction is carried over from SimpleSQOneElectronOperator.
    using Vectorizer = _Vectorizer;

    // The spinor tag corresponding to a `GSQTwoElectronOperator`.
    using SpinorTag = GeneralSpinorTag;


public:
    /*
     *  MARK: Constructors
     */

    // Inherit `SimpleSQOneElectronOperator`'s constructors.
    using SimpleSQTwoElectronOperator<_Scalar, _Vectorizer, GSQTwoElectronOperator<_Scalar, _Vectorizer>>::SimpleSQTwoElectronOperator;


    /*
     *  MARK: Named constructors
     */

    /**
     *  Construct a `GSQTwoElectronOperator` from a `PureUSQTwoElectronOperatorComponent`.
     * 
     *  @param g_component          The pure component of an unrestricted two-electron operator that should be converted.
     */
    static GSQTwoElectronOperator<Scalar, Vectorizer> FromUnrestrictedComponent(const PureUSQTwoElectronOperatorComponent<Scalar, Vectorizer>& g_component) {

        // We can just wrap the one-electron integrals into the correct class.
        const StorageArray<SquareRankFourTensor<Scalar>, Vectorizer> array {g_component.allParameters(), g_component.vectorizer()};
        return GSQTwoElectronOperator<Scalar, Vectorizer> {array};
    }
};


/*
 *  MARK: Convenience aliases
 */

// A scalar-like GSQTwoElectronOperator, i.e. with scalar-like access.
template <typename Scalar>
using ScalarGSQTwoElectronOperator = GSQTwoElectronOperator<Scalar, ScalarVectorizer>;

// A vector-like GSQTwoElectronOperator, i.e. with vector-like access.
template <typename Scalar>
using VectorGSQTwoElectronOperator = GSQTwoElectronOperator<Scalar, VectorVectorizer>;

// A matrix-like GSQTwoElectronOperator, i.e. with matrix-like access.
template <typename Scalar>
using MatrixGSQTwoElectronOperator = GSQTwoElectronOperator<Scalar, MatrixVectorizer>;

// A tensor-like GSQTwoElectronOperator, i.e. with tensor-like access.
template <typename Scalar, size_t N>
using TensorGSQTwoElectronOperator = GSQTwoElectronOperator<Scalar, TensorVectorizer<N>>;


/*
 *  MARK: Operator traits
 */

/**
 *  A type that provides compile-time information (traits) on `GSQTwoElectronOperator` that is otherwise not accessible through a public class alias.
 * 
 *  @tparam Scalar          The scalar type used for a single parameter/matrix element: real or complex.
 *  @tparam Vectorizer      The type of the vectorizer that relates a one-dimensional storage of tensors to the tensor structure of two-electron operators. This distinction is carried over from SimpleSQOneElectronOperator.
 */
template <typename Scalar, typename Vectorizer>
struct OperatorTraits<GSQTwoElectronOperator<Scalar, Vectorizer>> {

    // A type that corresponds to the scalar version of the associated general(ized) two-electron operator type.
    using ScalarOperator = ScalarGSQTwoElectronOperator<Scalar>;

    // The type of one-electron operator that is naturally related to a `GSQTwoElectronOperator`.
    using SQOneElectronOperator = GSQOneElectronOperator<Scalar, Vectorizer>;

    // The type of transformation that is naturally associated to a `GSQTwoElectronOperator`.
    using Transformation = GTransformation<Scalar>;

    // The type of density matrix that is naturally associated to a `GSQTwoElectronOperator`.
    using OneDM = G1DM<Scalar>;

    // The type of density matrix that is naturally associated to a `GSQTwoElectronOperator`.
    using TwoDM = G2DM<Scalar>;
};


/*
 *  MARK: BasisTransformableTraits
 */

/**
 *  A type that provides compile-time information related to the abstract interface `BasisTransformable`.
 * 
 *  @tparam Scalar          The scalar type used for a single parameter/matrix element: real or complex.
 *  @tparam Vectorizer      The type of the vectorizer that relates a one-dimensional storage of tensors to the tensor structure of two-electron operators. This distinction is carried over from SimpleSQOneElectronOperator.
 */
template <typename Scalar, typename Vectorizer>
struct BasisTransformableTraits<GSQTwoElectronOperator<Scalar, Vectorizer>> {

    // The type of transformation matrix that is naturally associated to a `GSQTwoElectronOperator`.
    using Transformation = GTransformation<Scalar>;
};


/*
 *  MARK: JacobiRotatableTraits
 */

/**
 *  A type that provides compile-time information related to the abstract interface `JacobiRotatable`.
 */
template <typename Scalar, typename Vectorizer>
struct JacobiRotatableTraits<GSQTwoElectronOperator<Scalar, Vectorizer>> {

    // The type of Jacobi rotation for which the Jacobi rotation should be defined.
    using JacobiRotationType = JacobiRotation;
};


}  // namespace GQCP
