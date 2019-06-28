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
#ifndef BaseAP1roGSolver_hpp
#define BaseAP1roGSolver_hpp


#include "HamiltonianParameters/HamiltonianParameters.hpp"
#include "Molecule.hpp"
#include "Geminals/AP1roGGeminalCoefficients.hpp"


namespace GQCP {


/**
 *  A base class for solvers using the AP1roG wave function
 */
class BaseAP1roGSolver {
protected:
    size_t K;  // the number of spatial orbitals
    size_t N_P;  // the number of electron pairs

    double convergence_threshold;  // the threshold used to check for convergence on the geminal coefficients
    size_t maximum_number_of_iterations;  // maximum number of Newton steps that may be used to achieve convergence of the PSEs

    double electronic_energy;  // the converged electronic energy

    AP1roGGeminalCoefficients geminal_coefficients;  // the converged geminal coefficients

    HamiltonianParameters<double> ham_par;


public:
    // CONSTRUCTORS

    /**
     *  @param N_P                                  the number of electrons
     *  @param ham_par                              Hamiltonian parameters in an orthonormal orbital basis
     *  @param G                                    the initial guess for the AP1roG gemial coefficients
     *  @param convergence_threshold                the threshold used to check for convergence on the geminal coefficients
     *  @param maximum_number_of_iterations         the maximum number of Newton steps that may be used to achieve convergence of the PSEs
     */
    BaseAP1roGSolver(size_t N_P, const HamiltonianParameters<double>& ham_par, const AP1roGGeminalCoefficients& G, const double convergence_threshold = 1.0e-08, const size_t maximum_number_of_iterations = 128);

    /**
     *  @param N_P                                  the number of electrons
     *  @param ham_par                              Hamiltonian parameters in an orthonormal orbital basis
     *  @param convergence_threshold                the threshold used to check for convergence on the geminal coefficients
     *  @param maximum_number_of_iterations         the maximum number of Newton steps that may be used to achieve convergence of the PSEs
     * 
     *  The initial guess for the geminal coefficients is zero
     */
    BaseAP1roGSolver(size_t N_P, const HamiltonianParameters<double>& ham_par, const double convergence_threshold = 1.0e-08, const size_t maximum_number_of_iterations = 128);

    /**
     *  @param molecule                             the molecule used for the AP1roG calculation
     *  @param ham_par                              Hamiltonian parameters in an orthonormal orbital basis
     *  @param G                                    the initial guess for the AP1roG gemial coefficients
     *  @param convergence_threshold                the threshold used to check for convergence on the geminal coefficients
     *  @param maximum_number_of_iterations         the maximum number of Newton steps that may be used to achieve convergence of the PSEs
     */
    BaseAP1roGSolver(const Molecule& molecule, const HamiltonianParameters<double>& ham_par, const AP1roGGeminalCoefficients& G, const double convergence_threshold = 1.0e-08, const size_t maximum_number_of_iterations = 128);

    /**
     *  @param molecule                             the molecule used for the AP1roG calculation
     *  @param ham_par                              Hamiltonian parameters in an orthonormal orbital basis
     *  @param convergence_threshold                the threshold used to check for convergence on the geminal coefficients
     *  @param maximum_number_of_iterations         the maximum number of Newton steps that may be used to achieve convergence of the PSEs
     *
     *  The initial guess for the geminal coefficients is zero
     */
    BaseAP1roGSolver(const Molecule& molecule, const HamiltonianParameters<double>& ham_par, const double convergence_threshold = 1.0e-08, const size_t maximum_number_of_iterations = 128);


    // DESTRUCTOR
    virtual ~BaseAP1roGSolver() = default;


    // GETTERS
    double get_electronic_energy() const { return this->electronic_energy; }
    const AP1roGGeminalCoefficients& get_geminal_coefficients() const { return this->geminal_coefficients; }
    const HamiltonianParameters<double>& get_ham_par() const { return this->ham_par; }


    // PUBLIC METHODS
    /**
     *  The actual 'solving' step
     */
    virtual void solve() = 0;
};


}  // namespace GQCP


#endif /* BaseAP1roGSolver_hpp */