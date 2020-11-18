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

#define BOOST_TEST_MODULE "QCMethod_RHF_stability_test"

#include <boost/test/unit_test.hpp>

#include "Operator/SecondQuantized/SQHamiltonian.hpp"
#include "QCMethod/HF/RHF/DiagonalRHFFockMatrixObjective.hpp"
#include "QCMethod/HF/RHF/RHF.hpp"
#include "QCMethod/HF/RHF/RHFSCFSolver.hpp"
#include "QCModel/HF/StabilityMatrices/RHFStabilityMatrices.hpp"


/**
 *  Check if our plain RHF SCF solver finds results (energy, orbital energies and coefficient matrix) that are equal to results from HORTON.
 */
BOOST_AUTO_TEST_CASE(h2o_sto3g_stability) {

    // Do our own RHF calculation
    const auto water = GQCP::Molecule::ReadXYZ("data/h2o.xyz");
    const GQCP::RSpinOrbitalBasis<double, GQCP::GTOShell> spinor_basis {water, "STO-3G"};
    const auto sq_hamiltonian = GQCP::RSQHamiltonian<double>::Molecular(spinor_basis, water);  // in an AO basis
    const GQCP::DiagonalRHFFockMatrixObjective<double> objective {sq_hamiltonian};

    auto rhf_environment = GQCP::RHFSCFEnvironment<double>::WithCoreGuess(water.numberOfElectrons(), sq_hamiltonian, spinor_basis.overlap().parameters());
    auto plain_rhf_scf_solver = GQCP::RHFSCFSolver<double>::Plain();
    const auto qc_structure = GQCP::QCMethod::RHF<double>().optimize(objective, plain_rhf_scf_solver, rhf_environment);
    auto rhf_parameters = qc_structure.groundStateParameters();

    // We can now check the stability of the ground state parameters.
    // Calculate the stability matrices.
    const auto stability_matrices = rhf_parameters.calculateStabilityMatrices(sq_hamiltonian);

    // This method should be internally stable.
    const auto internal_stability = stability_matrices.isInternallyStable();
    BOOST_CHECK(internal_stability == true);

    // This wavefunction should also be externally stable.
    const auto external_stability = stability_matrices.isExternallyStable();
    BOOST_CHECK(external_stability[0] == true);
    BOOST_CHECK(external_stability[1] == true);

    // Check that the stability properties can be printed
    stability_matrices.printStabilityDescription();
}
