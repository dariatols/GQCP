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
#define BOOST_TEST_MODULE "AP1roGPSESolver"

#include <boost/test/unit_test.hpp>

#include "Basis/transform.hpp"
#include "Geminals/AP1roGPSESolver.hpp"
#include "Operator/SecondQuantized/SQHamiltonian.hpp"
#include "RHF/PlainRHFSCFSolver.hpp"


BOOST_AUTO_TEST_CASE ( constructor ) {

    // Test a correct constructor
    auto h2 = GQCP::Molecule::ReadXYZ("data/h2_szabo.xyz");
    size_t N = 2;  // number of electrons for H2
    size_t N_P = N/2;  // number of electron pairs for H2
    GQCP::SingleParticleBasis<double, GQCP::GTOShell> sp_basis (h2, "STO-3G");
    auto sq_hamiltonian = GQCP::SQHamiltonian<double>::Molecular(sp_basis, h2);  // in an AO basis
    GQCP::AP1roGPSESolver ap1rog_pse_solver (N_P, sq_hamiltonian);
}


BOOST_AUTO_TEST_CASE ( constructor_molecule ) {

    // Test a correct constructor
    // Check if we can also pass a molecule object to the constructor
    auto h2 = GQCP::Molecule::ReadXYZ("data/h2_szabo.xyz");
    GQCP::SingleParticleBasis<double, GQCP::GTOShell> sp_basis (h2, "STO-3G");
    auto sq_hamiltonian = GQCP::SQHamiltonian<double>::Molecular(sp_basis, h2);  // in an AO basis

    // Test a faulty constructor
    auto h2_cation = GQCP::Molecule::ReadXYZ("data/h2_szabo.xyz", +1);
    BOOST_CHECK_THROW(GQCP::AP1roGPSESolver(h2_cation, sq_hamiltonian), std::invalid_argument);  // we can use the same Hamiltonian for molecule and ion
}


BOOST_AUTO_TEST_CASE ( h2_631gdp ) {

    // We have some reference data from olsens: H2 with HF/6-31G** orbitals
    //  AP1roG energy: -1.8696828608304892
    //  AP1roG coefficients: [-0.05949796, -0.05454253, -0.03709503, -0.02899231, -0.02899231, -0.01317386, -0.00852702, -0.00852702, -0.00517996]

    double ref_ap1rog_energy = -1.8696828608304892;
    GQCP::VectorX<double> ref_ap1rog_coefficients (9);
    ref_ap1rog_coefficients << -0.05949796, -0.05454253, -0.03709503, -0.02899231, -0.02899231, -0.01317386, -0.00852702, -0.00852702, -0.00517996;


    // Prepare molecular Hamiltonian in the RHF basis
    auto h2 = GQCP::Molecule::ReadXYZ("data/h2_olsens.xyz");
    GQCP::SingleParticleBasis<double, GQCP::GTOShell> sp_basis (h2, "6-31G**");
    auto sq_hamiltonian = GQCP::SQHamiltonian<double>::Molecular(sp_basis, h2);  // in an AO basis

    GQCP::PlainRHFSCFSolver plain_scf_solver (sq_hamiltonian, sp_basis, h2);
    plain_scf_solver.solve();
    auto rhf = plain_scf_solver.get_solution();
    GQCP::basisTransform(sp_basis, sq_hamiltonian, rhf.get_C());


    // Solve the AP1roG pSE equations with the initial guess being 0
    GQCP::AP1roGPSESolver ap1rog_pse_solver (h2, sq_hamiltonian);
    ap1rog_pse_solver.solve();

    double electronic_energy = ap1rog_pse_solver.get_electronic_energy();
    GQCP::VectorX<double> ap1rog_coefficients = ap1rog_pse_solver.get_geminal_coefficients().asVector();


    BOOST_CHECK(std::abs(electronic_energy - ref_ap1rog_energy) < 1.0e-05);

    for (size_t i = 0; i < 9; i++) {
        BOOST_CHECK(std::abs(ap1rog_coefficients(i) - ref_ap1rog_coefficients(i)) < 1.0e-05);
    }
}


BOOST_AUTO_TEST_CASE ( h2_631gdp_weak_interaction_limit ) {

    // We have some reference data from olsens: H2 with HF/6-31G** orbitals
    //  AP1roG energy: -1.8696828608304892
    //  AP1roG coefficients: [-0.05949796, -0.05454253, -0.03709503, -0.02899231, -0.02899231, -0.01317386, -0.00852702, -0.00852702, -0.00517996]

    double ref_ap1rog_energy = -1.8696828608304892;
    GQCP::VectorX<double> ref_ap1rog_coefficients (9);
    ref_ap1rog_coefficients << -0.05949796, -0.05454253, -0.03709503, -0.02899231, -0.02899231, -0.01317386, -0.00852702, -0.00852702, -0.00517996;


    // Prepare molecular Hamiltonian in the RHF basis
    auto h2 = GQCP::Molecule::ReadXYZ("data/h2_olsens.xyz");
    size_t N_P = h2.numberOfElectrons() / 2;
    GQCP::SingleParticleBasis<double, GQCP::GTOShell> sp_basis (h2, "6-31G**");
    auto sq_hamiltonian = GQCP::SQHamiltonian<double>::Molecular(sp_basis, h2);  // in an AO basis

    GQCP::PlainRHFSCFSolver plain_scf_solver (sq_hamiltonian, sp_basis, h2);
    plain_scf_solver.solve();
    auto rhf = plain_scf_solver.get_solution();
    GQCP::basisTransform(sp_basis, sq_hamiltonian, rhf.get_C());


    // Solve the AP1roG pSE equations, with the initial guess being the weak interaction limit coefficients
    GQCP::AP1roGPSESolver ap1rog_pse_solver (h2, sq_hamiltonian, GQCP::AP1roGGeminalCoefficients::WeakInteractionLimit(sq_hamiltonian, N_P));
    ap1rog_pse_solver.solve();

    double electronic_energy = ap1rog_pse_solver.get_electronic_energy();
    GQCP::VectorX<double> ap1rog_coefficients = ap1rog_pse_solver.get_geminal_coefficients().asVector();


    BOOST_CHECK(std::abs(electronic_energy - ref_ap1rog_energy) < 1.0e-05);

    for (size_t i = 0; i < 9; i++) {
        BOOST_CHECK(std::abs(ap1rog_coefficients(i) - ref_ap1rog_coefficients(i)) < 1.0e-05);
    }
}
