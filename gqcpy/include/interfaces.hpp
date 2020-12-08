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

#include "Basis/ScalarBasis/GTOShell.hpp"
#include "Basis/Transformations/BasisTransformable.hpp"
#include "Operator/FirstQuantized/Operator.hpp"
#include "Operator/SecondQuantized/OperatorTraits.hpp"
#include "QuantumChemical/SpinResolvedBase.hpp"
#include "gqcpy/include/utilities.hpp"

#include <pybind11/eigen.h>
#include <pybind11/functional.h>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>


namespace gqcpy {


// Provide some shortcuts for frequent namespaces.
namespace py = pybind11;
using namespace GQCP;


/**
 *  This header contains methods that bind the public interface of some C++ classes/interfaces to Pybind11 classes.
 */

/*
 *  MARK: Abstract interfaces
 */

/**
 *  Add Python bindings for the `BasisTransformable` interface.
 * 
 *  @tparam Class               The type of the Pybind11 `class_` (generated by the compiler).
 *  
 *  @param py_class             The Pybind11 `class_` that should obtain APIs related to `BasisTransformable` APIs.
 */
template <typename Class>
void bindBasisTransformableInterface(Class& py_class) {

    // The C++ type corresponding to the Python class.
    using Type = typename Class::type;
    using Transformation = typename BasisTransformableTraits<Type>::Transformation;

    py_class
        .def(
            "rotate",
            [](Type& transformable, const Transformation& U) {
                transformable.rotate(U);
            },
            py::arg("U"),
            "In-place apply the basis rotation.")

        .def(
            "rotated",
            [](const Type& transformable, const Transformation& U) {
                return transformable.rotated(U);
            },
            py::arg("U"),
            "Apply the basis rotation and return the result.")

        .def(
            "transform",
            [](Type& transformable, const Transformation& T) {
                transformable.transform(T);
            },
            py::arg("T"),
            "In-place apply the basis transformation.")

        .def(
            "transformed",
            [](Type& transformable, const Transformation& T) {
                return transformable.transformed(T);
            },
            py::arg("T"),
            "Apply the basis transformation and return the result.");
}


/**
 *  Add Python bindings for the `VectorSpaceArithmetic` C++ interface to a given `pybind11::class_`.
 * 
 *  @tparam Class               The type of the Pybind11 `class_` (generated by the compiler).
 *  
 *  @param py_class             The Pybind11 `class_` that should obtain `VectorSpaceArithmetic`'s APIs.
 */
template <typename Class>
void bindVectorSpaceArithmeticInterface(Class& py_class) {

    // The C++ type corresponding to the Python class.
    using Type = typename Class::type;
    using Scalar = typename Type::Scalar;

    py_class
        .def(py::self += py::self)
        .def(py::self + py::self)

        .def(py::self -= py::self)
        .def(py::self - py::self)

        .def(py::self *= Scalar())
        .def(Scalar() * py::self)
        .def(py::self * Scalar())

        .def(py::self / Scalar())

        .def(-py::self);
}


/**
 *  Add Python bindings for the `SpinResolvedBase` C++ interface to a given `pybind11::class_`.
 * 
 *  @tparam Class               The type of the Pybind11 `class_` (generated by the compiler).
 *  
 *  @param py_class             The Pybind11 `class_` that should obtain `SpinResolvedBase`'s APIs.
 */
template <typename Class>
void bindSpinResolvedBaseInterface(Class& py_class) {

    // The C++ type corresponding to the Python class.
    using Type = typename Class::type;

    py_class
        .def(
            "alpha",
            [](const Type& spin_resolved_object) {
                return spin_resolved_object.alpha();
            },
            "A read-only reference to the alpha object.")

        .def(
            "alpha",
            [](Type& spin_resolved_object) {
                return spin_resolved_object.alpha();
            },
            "A writable reference to the alpha object.")

        .def(
            "beta",
            [](const Type& spin_resolved_object) {
                return spin_resolved_object.beta();
            },
            "A read-only reference to the beta object.")

        .def(
            "beta",
            [](Type& spin_resolved_object) {
                return spin_resolved_object.beta();
            },
            "A writable reference to the beta object.")

        .def(
            "component",
            [](const Type& spin_resolved_object, const Spin sigma) {
                return spin_resolved_object.component(sigma);
            },
            py::arg("sigma"),
            "A read-only reference to the alpha or beta object.")

        .def(
            "component",
            [](Type& spin_resolved_object, const Spin sigma) {
                return spin_resolved_object.component(sigma);
            },
            py::arg("sigma"),
            "A writable reference to the alpha or beta object.");
}


/**
 *  Add some APIs related to operations on `BasisTransformable` objects.
 * 
 *  @tparam Class               The type of the Pybind11 `class_` (generated by the compiler).
 *  
 *  @param py_class             The Pybind11 `class_` that should obtain APIs related to `BasisTransformable` operations.
 */
template <typename Class>
void bindBasisTransformableOperationsInterface(Class& py_class) {

    // The C++ type corresponding to the Python class.
    using Type = typename Class::type;

    py_class
        .def(
            "inverse",
            &Type::inverse,
            "Return the inverse transformation of this transformation matrix.")

        .def(
            "isUnitary",
            &Type::isUnitary,
            py::arg("threshold") = 1.0e-12,
            "Return if this transformation is unitary, within the given threshold");
}


/**
 *  Add Python bindings for the `DoublySpinResolvedBase` C++ interface to a given `pybind11::class_`.
 * 
 *  @tparam Class               The type of the Pybind11 `class_` (generated by the compiler).
 *  
 *  @param py_class             The Pybind11 `class_` that should obtain `DoublySpinResolvedBase`'s APIs.
 */
template <typename Class>
void bindDoublySpinResolvedBaseInterface(Class& py_class) {

    // The C++ type corresponding to the Python class.
    using Type = typename Class::type;

    py_class
        .def(
            "alphaAlpha",
            [](const Type& spin_resolved_object) {
                return spin_resolved_object.alphaAlpha();
            },
            "A read-only reference to the alpha-alpha object.")

        .def(
            "alphaAlpha",
            [](Type& spin_resolved_object) {
                return spin_resolved_object.alphaAlpha();
            },
            "A writable reference to the alpha-alpha object.")

        .def(
            "alphaBeta",
            [](const Type& spin_resolved_object) {
                return spin_resolved_object.alphaBeta();
            },
            "A read-only reference to the alpha-beta object.")

        .def(
            "alphaBeta",
            [](Type& spin_resolved_object) {
                return spin_resolved_object.alphaBeta();
            },
            "A writable reference to the alpha-beta object.")

        .def(
            "betaAlpha",
            [](const Type& spin_resolved_object) {
                return spin_resolved_object.betaAlpha();
            },
            "A read-only reference to the beta-alpha object.")

        .def(
            "betaAlpha",
            [](Type& spin_resolved_object) {
                return spin_resolved_object.betaAlpha();
            },
            "A writable reference to the beta-alpha object.")

        .def(
            "betaBeta",
            [](const Type& spin_resolved_object) {
                return spin_resolved_object.betaBeta();
            },
            "A read-only reference to the beta-beta object.")

        .def(
            "betaBeta",
            [](Type& spin_resolved_object) {
                return spin_resolved_object.betaBeta();
            },
            "A writable reference to the beta-beta object.");
}


/*
 *  MARK: Basis - SpinorBasis
 */

/**
 *  Add Python bindings for some APIs related to spinor bases.
 * 
 *  @tparam Class               The type of the Pybind11 `class_` (generated by the compiler).
 *  
 *  @param py_class             The Pybind11 `class_` that should obtain APIs related to spinor bases.
 */
template <typename Class>
void bindSpinorBasisInterface(Class& py_class) {

    // The C++ type corresponding to the Python class.
    using Type = typename Class::type;

    py_class

        /*
         *  MARK: Coefficient access
         */

        .def(
            "expansion",
            &Type::expansion,
            "A read-only reference to the transformation that relates the current set of spinors with the atomic spinors.")


        /*
         *  MARK: Orthonormality
         */

        .def(
            "overlap",
            &Type::overlap,
            "The overlap (one-electron) operator of this spinor basis.")

        .def(
            "isOrthonormal",
            &Type::isOrthonormal,
            py::arg("precision") = 1.0e-08,
            "Return if this spinor basis is orthonormal within the given precision")

        .def(
            "lowdinOrthonormalization",
            &Type::lowdinOrthonormalization,
            "The transformation to the Löwdin basis: T = S_current^{-1/2}.")

        .def(
            "lowdinOrthonormalize",
            &Type::lowdinOrthonormalize,
            "Transform the spinor basis to the 'Löwdin basis', which is the orthonormal basis that we transform to with T = S^{-1/2}, where S is the current overlap matrix.");


    /*
     *  MARK: Conforming to `BasisTransformable`
     */
    bindBasisTransformableInterface(py_class);
}


/**
 *  Add Python bindings for the Mulliken partitioning for spinor bases.
 * 
 *  @tparam Class               The type of the Pybind11 `class_` (generated by the compiler).
 *  
 *  @param py_class             The Pybind11 `class_` that should obtain APIs related to Mulliken partitioning.
 */
template <typename Class>
void bindSpinorBasisMullikenInterface(Class& py_class) {

    // The C++ type corresponding to the Python class.
    using Type = typename Class::type;

    py_class.def(
        "mullikenPartitioning",
        [](const Type& spinor_basis, const std::function<bool(const GTOShell&)>& selector) {
            return spinor_basis.mullikenPartitioning(selector);
        },
        py::arg("selector"),
        "A Mulliken partitioning for the AOs selected by the supplied selector function.");
}


/**
 *  Add Python bindings for the quantization of first-quantized operators.
 * 
 *  @tparam Class               The type of the Pybind11 `class_` (generated by the compiler).
 *  
 *  @param py_class             The Pybind11 `class_` that should obtain APIs related to the quantization of first-quantized operators.
 */
template <typename Class>
void bindSpinorBasisQuantizationInterface(Class& py_class) {

    // The C++ type corresponding to the Python class.
    using Type = typename Class::type;

    py_class
        .def(
            "quantizeCoulombRepulsionOperator",
            [](const Type& spinor_basis) {
                return spinor_basis.quantize(Operator::Coulomb());
            },
            "Return the Coulomb operator expressed in this spinor basis.")

        .def(
            "quantizeKineticOperator",
            [](const Type& spinor_basis) {
                return spinor_basis.quantize(Operator::Kinetic());
            },
            "Return the kinetic energy operator expressed in this spinor basis.")

        .def(
            "quantizeNuclearAttractionOperator",
            [](const Type& spinor_basis, const Molecule& molecule) {
                return spinor_basis.quantize(Operator::NuclearAttraction(molecule));
            },
            "Return the nuclear attraction operator expressed in this spinor basis.")

        .def(
            "quantizeOverlapOperator",
            [](const Type& spinor_basis) {
                return spinor_basis.quantize(Operator::Overlap());
            },
            "Return the overlap operator expressed in this spinor basis.");
}


/*
 *  MARK: Basis - MullikenPartitioning
 */

/**
 *  Add Python bindings for the Mulliken indices to a type that represents a Mulliken partitioning.
 * 
 *  @tparam Class               The type of the Pybind11 `class_` (generated by the compiler).
 *  
 *  @param py_class             The Pybind11 `class_` that should obtain APIs related to Mulliken partitioning.
 */
template <typename Class>
void bindMullikenPartitioningIndicesInterface(Class& py_class) {

    // The C++ type corresponding to the Python class.
    using Type = typename Class::type;

    py_class.def("indices",
                 &Type::indices,
                 "Return the set of indices that correspond to the AOs that are included in the Mulliken-partitioning of an AO basis.");
}


/**
 *  Add Python bindings for the Mulliken matrices to a type that represents a Mulliken partitioning.
 * 
 *  @tparam Class               The type of the Pybind11 `class_` (generated by the compiler).
 *  
 *  @param py_class             The Pybind11 `class_` that should obtain APIs related to Mulliken partitioning.
 */
template <typename Class>
void bindMullikenPartitioningMatricesInterface(Class& py_class) {

    // The C++ type corresponding to the Python class.
    using Type = typename Class::type;

    py_class
        .def("partitionMatrix",
             &Type::partitionMatrix,
             "Return the partition matrix 'P_A' related to this Mulliken partitioning.")

        .def("projectionMatrix",
             &Type::projectionMatrix,
             "Return the Mulliken projection, defined as C^{-1} P_A C, where C is the transformation matrix and P_A is the partition matrix.");
}


/*
 * MARK: Basis - Transformations
 */

/**
 *  Add Python bindings for some APIs related to `SimpleTransformation`.
 * 
 *  @tparam Class               The type of the Pybind11 `class_` (generated by the compiler).
 *  
 *  @param py_class             The Pybind11 `class_` that should obtain APIs related to `SimpleTransformation`.
 */
template <typename Class>
void bindSimpleTransformationInterface(Class& py_class) {

    // The C++ type corresponding to the Python class.
    using Type = typename Class::type;
    using Scalar = typename Type::Scalar;


    py_class

        /*
         *  MARK: Constructors
         */

        .def(py::init<>([](const Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>& T) {
                 return Type {T};
             }),
             py::arg("T"))


        /*
         *  MARK: Transformation matrix
         */

        .def("matrix",
             &Type::matrix,
             "Return the transformation matrix that collects the expansion coefficients of the new basis (vectors) in the old basis as columns.");


    // Expose the `BasisTransformable` APIs.
    bindBasisTransformableInterface(py_class);

    // Add some APIs related to operations on `BasisTransformable` objects.
    bindBasisTransformableOperationsInterface(py_class);
}


/*
 *  MARK: Operator - SecondQuantized
 */


/**
 *  Add Python bindings for some Mulliken partitioning APIs to a given `pybind11::class_` that represents a one-electron operator.
 * 
 *  @tparam Class               The type of the Pybind11 `class_` (generated by the compiler).
 *  
 *  @param py_class             The Pybind11 `class_` (that represents a one-electron operator) that should obtain some Mulliken partitioning APIs.
 */
template <typename Class>
void bindSQOneElectronOperatorMullikenInterface(Class& py_class) {

    // The C++ type corresponding to the Python class.
    using Type = typename Class::type;

    py_class.def("partitioned",
                 &Type::partitioned,
                 "Partition this one-electron operator according to the supplied Mulliken partitioning scheme.");
}


/**
 *  Add Python bindings for APIs related to one-electron operators to a given `pybind11::class_`.
 * 
 *  @tparam Class               The type of the Pybind11 `class_` (generated by the compiler).
 *  
 *  @param py_class             The Pybind11 `class_` that should obtain one-electron operator APIs.
 */
template <typename Class>
void bindSQOneElectronOperatorInterface(Class& py_class) {

    // The C++ type corresponding to the Python class.
    using Type = typename Class::type;

    using OneDM = typename OperatorTraits<Type>::OneDM;


    py_class

        /*
         *  MARK: Calculations
         */

        .def(
            "calculateExpectationValue",
            [](const Type& op, const OneDM& D) {
                return op.calculateExpectationValue(D).elements();
            },
            "Calculate the expectation value of this one-electron operator.");


    // Expose the `VectorSpaceArithmetic` APIs.
    bindVectorSpaceArithmeticInterface(py_class);

    // Expose the `BasisTransformable` APIs.
    bindBasisTransformableInterface(py_class);

    // Expose the Mulliken-partitioning APIs.
    bindSQOneElectronOperatorMullikenInterface(py_class);
}


/**
 *  Add Python bindings for the `SimpleSQOneElectronOperator` C++ interface to a given `pybind11::class_`.
 * 
 *  @tparam Class               The type of the Pybind11 `class_` (generated by the compiler).
 *  
 *  @param py_class             The Pybind11 `class_` that should obtain `SimpleSQOneElectronOperator`'s APIs.
 */
template <typename Class>
void bindSimpleSQOneElectronOperatorInterface(Class& py_class) {

    // The C++ type corresponding to the Python class.
    using Type = typename Class::type;

    using OneDM = typename OperatorTraits<Type>::OneDM;


    py_class

        /*
         *  MARK: Parameter access
         */

        .def(
            "allParameters",
            [](const Type& op) {
                return op.allParameters();
            },
            "A vector of read-only matrix representions of the parameters/matrix elements/integrals of this operator.");


    // Expose general one-electron operator APIs.
    bindSQOneElectronOperatorInterface(py_class);
}


/**
 *  Add Python bindings for the parameter access for scalar one-electron operators.
 * 
 *  @tparam Class               The type of the Pybind11 `class_` (generated by the compiler).
 *  
 *  @param py_class             The Pybind11 `class_` that should obtain acess bindings like a scalar one-electron operator.
 */
template <typename Class>
void bindScalarSQOneElectronOperatorParameterInterface(Class& py_class) {

    // The C++ type corresponding to the Python class.
    using Type = typename Class::type;

    py_class
        .def(
            "parameters",
            [](const Type& op) {
                return op.parameters();
            },
            "A read-only matrix representation of the parameters/matrix elements/integrals of one of the tensor components of this operator.");
}


/**
 *  Add Python bindings for the some of the two-electron operator interface to a given `pybind11::class_`.
 * 
 *  @tparam Class               The type of the Pybind11 `class_` (generated by the compiler).
 *  
 *  @param py_class             The Pybind11 `class_` that should obtain some two-electron operator APIs.
 */
template <typename Class>
void bindSQTwoElectronOperatorInterface(Class& py_class) {

    // The C++ type corresponding to the Python class.
    using Type = typename Class::type;

    using TwoDM = typename OperatorTraits<Type>::TwoDM;


    py_class
        /*
         *  MARK: Calculations
         */

        .def(
            "calculateExpectationValue",
            [](const Type& op, const TwoDM& D) {
                return op.calculateExpectationValue(D).elements();
            },
            "Calculate the expectation value of this one-electron operator.");


    // Expose the `VectorSpaceArithmetic` APIs.
    bindVectorSpaceArithmeticInterface(py_class);
}


/**
 *  Add Python bindings for the `SimpleSQTwoElectronOperator` C++ interface to a given `pybind11::class_`.
 * 
 *  @tparam Class               The type of the Pybind11 `class_` (generated by the compiler).
 *  
 *  @param py_class             The Pybind11 `class_` that should obtain `SimpleSQTwoElectronOperator`'s APIs.
 */
template <typename Class>
void bindSimpleSQTwoElectronOperatorInterface(Class& py_class) {

    // The C++ type corresponding to the Python class.
    using Type = typename Class::type;

    using TwoDM = typename OperatorTraits<Type>::TwoDM;


    // Add some two-electron operator APIs.
    bindSQTwoElectronOperatorInterface(py_class);

    // Expose the `BasisTransformable` APIs.
    bindBasisTransformableInterface(py_class);
}


/**
 *  Add Python bindings for the parameter access for scalar two-electron operators.
 * 
 *  @tparam Class               The type of the Pybind11 `class_` (generated by the compiler).
 *  
 *  @param py_class             The Pybind11 `class_` that should obtain access bindings like a scalar two-electron operator.
 */
template <typename Class>
void bindScalarSQTwoElectronOperatorParameterInterface(Class& py_class) {

    // The C++ type corresponding to the Python class.
    using Type = typename Class::type;

    py_class
        .def(
            "parameters",
            [](const Type& op) {
                return asNumpyArray(op.parameters().Eigen());
            },
            "A read-only matrix representation of the parameters/matrix elements/integrals of one of the tensor components of this operator.");
}


/*
 *  MARK: DensityMatrix
 */

/**
 *  Add Python bindings for some APIs related to `Simple2DM`.
 * 
 *  @tparam Class               The type of the Pybind11 `class_` (generated by the compiler).
 *  
 *  @param py_class             The Pybind11 `class_` that should obtain APIs related to `Simple2DM`.
 */
template <typename Class>
void bindSimple2DMInterface(Class& py_class) {

    // The C++ type corresponding to the Python class.
    using Type = typename Class::type;
    using Scalar = typename Type::Scalar;

    py_class
        .def(
            "reduce",
            &Type::reduce,
            "Return a partial contraction of the 2-DM, where D(p,q) = d(p,q,r,r).")

        .def(
            "trace",
            &Type::trace,
            "Return the trace of the 2-DM, i.e. d(p,p,q,q).");
}


}  // namespace gqcpy
