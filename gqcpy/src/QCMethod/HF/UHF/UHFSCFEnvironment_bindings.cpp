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

#include "QCMethod/HF/UHF/UHFSCFEnvironment.hpp"

#include <pybind11/eigen.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>


namespace gqcpy {


// Provide some shortcuts for frequent namespaces.
namespace py = pybind11;
using namespace GQCP;


void bindUHFSCFEnvironment(py::module& module) {
    py::class_<UHFSCFEnvironment<double>>(module, "UHFSCFEnvironment", "An algorithm environment that can be used with standard UHF SCF solvers.")

        // CONSTRUCTORS

        .def(py::init([](const size_t N_alpha, const size_t N_beta, const USQHamiltonian<double>& sq_hamiltonian, const ScalarUSQOneElectronOperator<double>& S, const UTransformation<double>& C_initial) {
                 return UHFSCFEnvironment<double>(N_alpha, N_beta, sq_hamiltonian, S, UTransformationComponent<double>(C_alpha_initial), UTransformationComponent<double>(C_beta_initial));
             }),
             py::arg("N_alpha"),
             py::arg("N_beta"),
             py::arg("sq_hamiltonian"),
             py::arg("S"),
             py::arg("C_initial"),
             "A constructor that initializes the environment with initial guesses for the alpha and beta coefficient matrices.")

        .def(py::init([](const QCModel::RHF<double>& rhf_parameters, const USQHamiltonian<double>& sq_hamiltonian, const ScalarUSQOneElectronOperator<double>& S) {
                 return UHFSCFEnvironment<double>(rhf_parameters, sq_hamiltonian, S);
             }),
             py::arg("rhf_parameters"),
             py::arg("sq_hamiltonian"),
             py::arg("S"),
             "A constructor that initializes the environment from converged RHF model parameters.")

        .def_static(
            "WithCoreGuess",
            [](const size_t N_alpha, const size_t N_beta, const USQHamiltonian<double>& sq_hamiltonian, const ScalarUSQOneElectronOperator<double>& S) {
                return UHFSCFEnvironment<double>::WithCoreGuess(N_alpha, N_beta, sq_hamiltonian, S);
            },
            "Initialize an UHF SCF environment with initial coefficient matrices (equal for alpha and beta) that is obtained by diagonalizing the core Hamiltonian matrix.")


        // Bind read-write members/properties, exposing intermediary environment variables to the Python interface.
        .def_readwrite("N", &UHFSCFEnvironment<double>::N)

        .def_readwrite("electronic_energies", &UHFSCFEnvironment<double>::electronic_energies)

        .def_readwrite("orbital_energies", &UHFSCFEnvironment<double>::orbital_energies)

        .def_property(
            "S",
            [](const UHFSCFEnvironment<double>& environment) {
                return environment.S;
            },
            [](UHFSCFEnvironment<double>& environment, const ScalarUSQOneElectronOperator<double>& S) {
                environment.S = S;
            })


        // Define read-only 'getters'
        .def_readonly(
            "density_matrices",
            &UHFSCFEnvironment<double>::density_matrices)

        .def_readonly(
            "error_vectors",
            &UHFSCFEnvironment<double>::error_vectors)


        // Define getters for non-native components
        .def(
            "coefficient_matrices_alpha",
            [](const UHFSCFEnvironment<double>& environment) {
                std::vector<UTransformationComponent<double>> coefficient_matrices_alpha;
                std::transform(environment.coefficient_matrices.begin(), environment.coefficient_matrices.end(), std::back_inserter(coefficient_matrices_alpha), [](const UTransformation<double>& C) { return C.alpha(); });

                return coefficient_matrices_alpha;
            })

        .def(
            "coefficient_matrices_beta",
            [](const UHFSCFEnvironment<double>& environment) {
                std::vector<UTransformationComponent<double>> coefficient_matrices_beta;
                std::transform(environment.coefficient_matrices.begin(), environment.coefficient_matrices.end(), std::back_inserter(coefficient_matrices_beta), [](const UTransformation<double>& C) { return C.beta(); });

                return coefficient_matrices_beta;
            })

        .def(
            "density_matrices_alpha",
            [](const UHFSCFEnvironment<double>& environment) {
                std::vector<MatrixX<double>> alpha_density_matrices;
                std::transform(environment.density_matrices.begin(), environment.density_matrices.end(), std::back_inserter(alpha_density_matrices), [](const SpinResolved1DM<double>& D) { return D.alpha(); });

                return alpha_density_matrices;
            })

        .def(
            "density_matrices_beta",
            [](const UHFSCFEnvironment<double>& environment) {
                std::vector<MatrixX<double>> beta_density_matrices;
                std::transform(environment.density_matrices.begin(), environment.density_matrices.end(), std::back_inserter(beta_density_matrices), [](const SpinResolved1DM<double>& D) { return D.beta(); });

                return beta_density_matrices;
            })

        .def(
            "fock_matrices_alpha",
            [](const UHFSCFEnvironment<double>& environment) {
                std::vector<MatrixX<double>> alpha_fock_matrices;
                std::transform(environment.fock_matrices.begin(), environment.fock_matrices.end(), std::back_inserter(alpha_fock_matrices), [](const ScalarUSQOneElectronOperator<double>& F) { return F.alpha().parameters(); });

                return alpha_fock_matrices;
            })

        .def(
            "fock_matrices_beta",
            [](const UHFSCFEnvironment<double>& environment) {
                std::vector<MatrixX<double>> beta_fock_matrices;
                std::transform(environment.fock_matrices.begin(), environment.fock_matrices.end(), std::back_inserter(beta_fock_matrices), [](const ScalarUSQOneElectronOperator<double>& F) { return F.beta().parameters(); });

                return beta_fock_matrices;
            })

        // Bind methods for the replacement of the most current iterates.
        .def("replace_current_coefficient_matrix_alpha",
             [](UHFSCFEnvironment<double>& environment, const Eigen::MatrixXd& new_coefficient_matrix_alpha) {
                 const auto last_spin_resolved_C = environment.coefficient_matrices.back();
                 const auto last_C_beta = last_spin_resolved_C.beta();
                 environment.coefficient_matrices.pop_back();
                 environment.coefficient_matrices.push_back(UTransformation<double>(UTransformationComponent<double>(new_coefficient_matrix_alpha), last_C_beta));
             })

        .def("replace_current_coefficient_matrix_beta",
             [](UHFSCFEnvironment<double>& environment, const Eigen::MatrixXd& new_coefficient_matrix_beta) {
                 const auto last_spin_resolved_C = environment.coefficient_matrices.back();
                 const auto last_C_alpha = last_spin_resolved_C.alpha();
                 environment.coefficient_matrices.pop_back();
                 environment.coefficient_matrices.push_back(UTransformation<double>(last_C_alpha, UTransformationComponent<double>(new_coefficient_matrix_beta)));
             })

        .def("replace_current_density_matrix_alpha",
             [](UHFSCFEnvironment<double>& environment, const Eigen::MatrixXd& new_density_matrix_alpha) {
                 const auto last_spin_resolved_density = environment.density_matrices.back();
                 const auto last_density_matrix_beta = last_spin_resolved_density.beta();
                 environment.density_matrices.pop_back();
                 environment.density_matrices.push_back(SpinResolved1DM<double>(new_density_matrix_alpha, last_density_matrix_beta));
             })

        .def("replace_current_density_matrix_beta",
             [](UHFSCFEnvironment<double>& environment, const Eigen::MatrixXd& new_density_matrix_beta) {
                 const auto last_spin_resolved_density = environment.density_matrices.back();
                 const auto last_density_matrix_alpha = last_spin_resolved_density.beta();
                 environment.density_matrices.pop_back();
                 environment.density_matrices.push_back(SpinResolved1DM<double>(last_density_matrix_alpha, new_density_matrix_beta));
             })

        .def("replace_current_fock_matrix_alpha",
             [](UHFSCFEnvironment<double>& environment, const Eigen::MatrixXd& new_fock_matrix_alpha) {
                 const auto last_USQ_Fock = environment.fock_matrices.back();
                 const auto last_fock_matrix_beta = last_USQ_Fock.beta().parameters();
                 ScalarUSQOneElectronOperator<double> new_fock {new_fock_matrix_alpha, last_fock_matrix_beta};

                 environment.fock_matrices.pop_back();
                 environment.fock_matrices.push_back(new_fock);
             })

        .def("replace_current_fock_matrix_beta",
             [](UHFSCFEnvironment<double>& environment, const Eigen::MatrixXd& new_fock_matrix_beta) {
                 const auto last_USQ_Fock = environment.fock_matrices.back();
                 const auto last_fock_matrix_alpha = last_USQ_Fock.alpha().parameters();
                 ScalarUSQOneElectronOperator<double> new_fock {last_fock_matrix_alpha, new_fock_matrix_beta};

                 environment.fock_matrices.pop_back();
                 environment.fock_matrices.push_back(new_fock);
             })

        .def("replace_current_error_vectors_alpha",
             [](UHFSCFEnvironment<double>& environment, const Eigen::MatrixXd& new_error_vectors_alpha) {
                 const auto last_spinResolved_error = environment.error_vectors.back();
                 const auto last_error_vector_beta = last_spinResolved_error.beta();
                 SpinResolved<VectorX<double>> new_error {new_error_vectors_alpha, last_error_vector_beta};

                 environment.error_vectors.pop_back();
                 environment.error_vectors.push_back(new_error);
             })

        .def("replace_current_error_vectors_beta",
             [](UHFSCFEnvironment<double>& environment, const Eigen::MatrixXd& new_error_vectors_beta) {
                 const auto last_spinResolved_error = environment.error_vectors.back();
                 const auto last_error_vector_alpha = last_spinResolved_error.alpha();
                 SpinResolved<VectorX<double>> new_error {last_error_vector_alpha, new_error_vectors_beta};

                 environment.error_vectors.pop_back();
                 environment.error_vectors.push_back(new_error);
             });
}


}  // namespace gqcpy
