// This file is part of GQCG-gqcp.
// 
// Copyright (C) 2017-2019  the GQCG developers
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
#define BOOST_TEST_MODULE "FCI_RDM_test"

#include <boost/test/unit_test.hpp>

#include "Processing/RDM/FCIRDMBuilder.hpp"

#include "Operator/SecondQuantized/SQHamiltonian.hpp"
#include "Processing/Properties/expectation_values.hpp"
#include "Processing/RDM/RDMCalculator.hpp"
#include "QCMethod/CI/CISolver.hpp"
#include "QCMethod/CI/HamiltonianBuilder/FCI.hpp"


BOOST_AUTO_TEST_CASE ( H2O_1RDM_spin_trace_FCI ) {

    // Test if the trace of the 1-RDMs (spin summed, aa and bb) gives N, N_a and N_b

    // Get the 1-RDM from FCI
    size_t N_a = 5;
    size_t N_b = 5;

    // Create the molecular Hamiltonian in the AO basis
    auto h2o = GQCP::Molecule::ReadXYZ("data/h2o_Psi4_GAMESS.xyz");
    GQCP::RSpinorBasis<double, GQCP::GTOShell> spinor_basis (h2o, "STO-3G");
    auto sq_hamiltonian = GQCP::SQHamiltonian<double>::Molecular(spinor_basis, h2o);  // in an AO basis
    size_t K = sq_hamiltonian.dimension();  // SO 7

    GQCP::ProductONVBasis fock_space (K, N_a, N_b);  // dim = 441
    GQCP::FCI fci (fock_space);

    // Specify solver options and solve the eigenvalue problem
    // Solve the dense FCI eigenvalue problem
    GQCP::CISolver ci_solver (fci, sq_hamiltonian);
    GQCP::DenseSolverOptions solver_options;
    ci_solver.solve(solver_options);

    GQCP::VectorX<double> coef = ci_solver.get_eigenpair().get_eigenvector();

    // Check if the FCI 1-RDMs have the proper trace.
    GQCP::FCIRDMBuilder fci_rdm (fock_space);
    GQCP::OneRDMs<double> one_rdms = fci_rdm.calculate1RDMs(coef);


    BOOST_CHECK(std::abs(one_rdms.one_rdm_aa.trace() - N_a) < 1.0e-12);
    BOOST_CHECK(std::abs(one_rdms.one_rdm_bb.trace() - N_b) < 1.0e-12);
    BOOST_CHECK(std::abs(one_rdms.one_rdm.trace() - (N_a + N_b)) < 1.0e-12);
}


BOOST_AUTO_TEST_CASE ( H2O_2RDM_spin_trace_FCI ) {

    // Test if the traces of the spin-resolved 2-RDMs (d_ppqq) gives the correct number
    
    size_t N_a = 5;
    size_t N_b = 5;

    // Create the molecular Hamiltonian in the AO basis
    auto h2o = GQCP::Molecule::ReadXYZ("data/h2o_Psi4_GAMESS.xyz");
    GQCP::RSpinorBasis<double, GQCP::GTOShell> spinor_basis (h2o, "STO-3G");
    auto sq_hamiltonian = GQCP::SQHamiltonian<double>::Molecular(spinor_basis, h2o);  // in an AO basis
    size_t K = sq_hamiltonian.dimension();  // SO 7

    GQCP::ProductONVBasis fock_space (K, N_a, N_b);  // dim = 441
    GQCP::FCI fci (fock_space);

    // Specify solver options and solve the eigenvalue problem
    // Solve the dense FCI eigenvalue problem
    GQCP::CISolver ci_solver (fci, sq_hamiltonian);
    GQCP::DenseSolverOptions solver_options;
    ci_solver.solve(solver_options);

    GQCP::VectorX<double> coef = ci_solver.get_eigenpair().get_eigenvector();

    // Check if the FCI 2-RDMs have the proper trace.
    GQCP::FCIRDMBuilder fci_rdm (fock_space);
    GQCP::TwoRDMs<double> two_rdms = fci_rdm.calculate2RDMs(coef);


    BOOST_CHECK(std::abs(two_rdms.two_rdm_aaaa.trace() - N_a*(N_a-1)) < 1.0e-12);
    BOOST_CHECK(std::abs(two_rdms.two_rdm_aabb.trace() - N_a*N_b) < 1.0e-12);
    BOOST_CHECK(std::abs(two_rdms.two_rdm_bbaa.trace() - N_b*N_a) < 1.0e-12);
    BOOST_CHECK(std::abs(two_rdms.two_rdm_bbbb.trace() - N_b*(N_b-1)) < 1.0e-12);
}


BOOST_AUTO_TEST_CASE ( H2O_1RDM_2RDM_trace_FCI ) {

    // Test if the relevant 2-RDM trace gives the 1-RDM for FCI

    size_t N_a = 5;
    size_t N_b = 5;
    size_t N = N_a + N_b;

    // Create the molecular Hamiltonian in the AO basis
    auto h2o = GQCP::Molecule::ReadXYZ("data/h2o_Psi4_GAMESS.xyz");
    GQCP::RSpinorBasis<double, GQCP::GTOShell> spinor_basis (h2o, "STO-3G");
    auto sq_hamiltonian = GQCP::SQHamiltonian<double>::Molecular(spinor_basis, h2o);  // in an AO basis
    size_t K = sq_hamiltonian.dimension();  // SO 7

    GQCP::ProductONVBasis fock_space (K, N_a, N_b);  // dim = 441
    GQCP::FCI fci (fock_space);

    // Specify solver options and solve the eigenvalue problem
    // Solve the dense FCI eigenvalue problem
    GQCP::CISolver ci_solver (fci, sq_hamiltonian);
    GQCP::DenseSolverOptions solver_options;
    ci_solver.solve(solver_options);

    GQCP::VectorX<double> coef = ci_solver.get_eigenpair().get_eigenvector();

    // Check if the 2-RDM contraction matches the reduction.
    GQCP::FCIRDMBuilder fci_rdm (fock_space);
    GQCP::TwoRDMs<double> two_rdms = fci_rdm.calculate2RDMs(coef);
    GQCP::OneRDMs<double> one_rdms = fci_rdm.calculate1RDMs(coef);


    GQCP::OneRDM<double> D_from_reduction = (1.0/(N-1)) * two_rdms.two_rdm.reduce();
    BOOST_CHECK(one_rdms.one_rdm.isApprox(D_from_reduction, 1.0e-12));
}


BOOST_AUTO_TEST_CASE ( H2O_energy_RDM_contraction_FCI ) {

    // Test if the contraction of the 1- and 2-RDMs with the one- and two-electron integrals gives the FCI energy

    size_t N_a = 5;
    size_t N_b = 5;

    // Create the molecular Hamiltonian in the AO basis
    auto h2o = GQCP::Molecule::ReadXYZ("data/h2o_Psi4_GAMESS.xyz");
    GQCP::RSpinorBasis<double, GQCP::GTOShell> spinor_basis (h2o, "STO-3G");
    auto sq_hamiltonian = GQCP::SQHamiltonian<double>::Molecular(spinor_basis, h2o);  // in an AO basis
    size_t K = sq_hamiltonian.dimension();  // SO 7

    GQCP::ProductONVBasis fock_space (K, N_a, N_b);  // dim = 441
    GQCP::FCI fci (fock_space);

    // Specify solver options and solve the eigenvalue problem
    // Solve the dense FCI eigenvalue problem
    GQCP::CISolver ci_solver (fci, sq_hamiltonian);
    GQCP::DenseSolverOptions solver_options;
    ci_solver.solve(solver_options);

    GQCP::VectorX<double> coef = ci_solver.get_eigenpair().get_eigenvector();
    double energy_by_eigenvalue = ci_solver.get_eigenpair().get_eigenvalue();

    // Check if the contraction energy matches the fci eigenvalue.
    GQCP::FCIRDMBuilder fci_rdm (fock_space);
    GQCP::TwoRDMs<double> two_rdms = fci_rdm.calculate2RDMs(coef);
    GQCP::OneRDMs<double> one_rdms = fci_rdm.calculate1RDMs(coef);

    double energy_by_contraction = sq_hamiltonian.calculateExpectationValue(one_rdms.one_rdm, two_rdms.two_rdm);

    BOOST_CHECK(std::abs(energy_by_eigenvalue - energy_by_contraction) < 1.0e-12);
}


BOOST_AUTO_TEST_CASE ( H2O_energy_RDM_contraction_FCI_LinearExpansion ) {

    // Repeat the contraction with an FCI wave function as input and the RDMCalculator API.

    size_t N_a = 5;
    size_t N_b = 5;

    // Create the molecular Hamiltonian in the AO basis
    auto h2o = GQCP::Molecule::ReadXYZ("data/h2o_Psi4_GAMESS.xyz");
    GQCP::RSpinorBasis<double, GQCP::GTOShell> spinor_basis (h2o, "STO-3G");
    auto sq_hamiltonian = GQCP::SQHamiltonian<double>::Molecular(spinor_basis, h2o);  // in an AO basis
    size_t K = sq_hamiltonian.dimension();  // SO 7

    GQCP::ProductONVBasis fock_space (K, N_a, N_b);  // dim = 441
    GQCP::FCI fci (fock_space);

    // Specify solver options and solve the eigenvalue problem
    // Solve the dense FCI eigenvalue problem
    GQCP::CISolver ci_solver (fci, sq_hamiltonian);
    GQCP::DenseSolverOptions solver_options;
    ci_solver.solve(solver_options);

    GQCP::LinearExpansion linear_expansion = ci_solver.makeLinearExpansion();
    double energy_by_eigenvalue = ci_solver.get_eigenpair().get_eigenvalue();

    // Check if the contraction energy matches the fci eigenvalue.
    GQCP::RDMCalculator fci_rdm (linear_expansion);
    GQCP::TwoRDMs<double> two_rdms = fci_rdm.calculate2RDMs();
    GQCP::OneRDMs<double> one_rdms = fci_rdm.calculate1RDMs();

    double energy_by_contraction = sq_hamiltonian.calculateExpectationValue(one_rdms.one_rdm, two_rdms.two_rdm);

    BOOST_CHECK(std::abs(energy_by_eigenvalue - energy_by_contraction) < 1.0e-12);
}


BOOST_AUTO_TEST_CASE ( throw_calculate_element ) {

    // Create a test wave function

    size_t K = 3;
    size_t N = 2;
    GQCP::ProductONVBasis fock_space (K, N, N);

    GQCP::VectorX<double> coeff (fock_space.get_dimension());
    coeff << 1, 1, -2, 4, -5, -6, 7, 9, 8;

    // not implemented yet and should throw
    GQCP::FCIRDMBuilder fci_rdm (fock_space);
    BOOST_CHECK_THROW(fci_rdm.calculateElement({0,0,1}, {1,0,2}, coeff), std::runtime_error);
}


/*
 *  Compare the RDMs calculated from a product ONV basis and an equivalent selected ONV basis
 */ 
BOOST_AUTO_TEST_CASE ( H2O_FCI_LinearExpansion_vs_Selected_CI ) {

    size_t N_a = 4;
    size_t N_b = 6;

    // Create the molecular Hamiltonian in the AO basis
    auto h2o = GQCP::Molecule::ReadXYZ("data/h2o_Psi4_GAMESS.xyz");
    GQCP::RSpinorBasis<double, GQCP::GTOShell> spinor_basis (h2o, "STO-3G");
    auto sq_hamiltonian = GQCP::SQHamiltonian<double>::Molecular(spinor_basis, h2o);  // in an AO basis
    size_t K = sq_hamiltonian.dimension();  // SO 7

    GQCP::ProductONVBasis fock_space (K, N_a, N_b);  // dim = 441
    GQCP::SelectedONVBasis selected_fock_space (fock_space);  // dim = 441
    GQCP::FCI fci (fock_space);

    // Specify solver options and solve the eigenvalue problem
    // Solve the dense FCI eigenvalue problem
    GQCP::CISolver ci_solver (fci, sq_hamiltonian);
    GQCP::DenseSolverOptions solver_options;
    ci_solver.solve(solver_options);

    GQCP::LinearExpansion linear_expansion = ci_solver.makeLinearExpansion();
    double energy_by_eigenvalue = ci_solver.get_eigenpair().get_eigenvalue();

    GQCP::RDMCalculator fci_rdm (linear_expansion);
    GQCP::RDMCalculator selected_rdm (selected_fock_space);
    selected_rdm.set_coefficients(linear_expansion.get_coefficients());
    
    GQCP::TwoRDMs<double> two_rdms = fci_rdm.calculate2RDMs();
    GQCP::OneRDMs<double> one_rdms = fci_rdm.calculate1RDMs();
    GQCP::TwoRDMs<double> two_rdms_selected = selected_rdm.calculate2RDMs();
    GQCP::OneRDMs<double> one_rdms_selected = selected_rdm.calculate1RDMs();

    BOOST_CHECK(two_rdms.two_rdm_aaaa.isApprox(two_rdms_selected.two_rdm_aaaa));
    BOOST_CHECK(two_rdms.two_rdm_aabb.isApprox(two_rdms_selected.two_rdm_aabb));
    BOOST_CHECK(two_rdms.two_rdm_bbaa.isApprox(two_rdms_selected.two_rdm_bbaa));
    BOOST_CHECK(two_rdms.two_rdm_bbbb.isApprox(two_rdms_selected.two_rdm_bbbb));

    BOOST_CHECK(one_rdms.one_rdm_aa.isApprox(one_rdms_selected.one_rdm_aa));
    BOOST_CHECK(one_rdms.one_rdm_bb.isApprox(one_rdms_selected.one_rdm_bb));
}
