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


#include "Molecule/Molecule.hpp"
#include "Operator/SecondQuantized/SQHamiltonian.hpp"


namespace GQCP {


/**
 *  A class that holds the collection of Hamiltonians that represent different molecular decompositions
 */
class AtomicDecompositionParameters {
private:
    RSQHamiltonian<double> molecular_hamiltonian;  // the Hamiltonian of the complete molecule

    std::vector<RSQHamiltonian<double>> net_atomic_parameters;   // the collection of net atomic Hamiltonians
    std::vector<RSQHamiltonian<double>> interaction_parameters;  // the collection of atomic interaction Hamiltonians
    std::vector<RSQHamiltonian<double>> atomic_parameters;       // the collection of atomic Hamiltonians


public:
    /*
     *  CONSTRUCTORS
     */

    /**
     *  @param molecular_hamiltonian                the complete molecular Hamiltonian
     *  @param net_atomic_parameters                the collection of net atomic Hamiltonians
     *  @param interaction_parameters               the collection of atomic interaction Hamiltonians
     *  @param atomic_parameters                    the collection of atomic Hamiltonians
     */
    AtomicDecompositionParameters(const RSQHamiltonian<double>& molecular_hamiltonian, const std::vector<RSQHamiltonian<double>>& net_atomic_parameters, const std::vector<RSQHamiltonian<double>>& interaction_parameters, const std::vector<RSQHamiltonian<double>>& atomic_parameters);

    /**
     *  The default constructor.
     */
    AtomicDecompositionParameters() = default;


    /*
     *  NAMED CONSTRUCTORS
     */

    /**
     *  Constructs net atomic, atomic and atomic interaction Hamiltonians in the AO basis for a diatomic molecule AB.
     *   the term "Nuclear" concerns how the electronic nuclear integrals (potential energy) are decomposed. The potential energy
     *   for basis functions on atom A for the charge on B are included in the interaction energy and not in the net atomic energy.
     *
     *  @param molecule     the molecule for which the AtomicDecompositionParameters should be calculated
     *  @param basisset     the name of the basisset corresponding to the AO basis
     *
     *  @return Atomic decomposed parameters:
     *      - net atomic parameters with:
     *          - one-electron nuclear integrals separated by atomic core and the atomic basis functions centered on that atom.
     *          - one-electron kinetic integrals separated per set of atomic basis functions centered on an atom.
     *          - two-electron integrals separated per set of atomic basis functions centered on an atom.
     *      - interaction parameters with:
     *          - remaining one- and two-electron contributions when deducting the net atomic parameters from the total Hamiltonian
     *          - scalar : nuclear repulsion
     *      - atomic parameters, HamiltonianParameters with:
     *          - net atomic parameters + interaction parameters/2
     *
     *  Ordering of the atomic Hamiltonians are dependant on the ordering of the atoms in the molecule
     *   for the molecule AB:
     *      net_atomic_parameters will contains parameters for A then B.
     *      interaction_parameters will contain parameters for the AB interaction.
     *      atomic_parameters will contain parameters for A then B.
     */
    static AtomicDecompositionParameters Nuclear(const Molecule& molecule, const std::string& basisset_name);


    /*
     *  PUBLIC METHODS
     */

    /**
     *  @return the collection of atomic Hamiltonians
     */
    const std::vector<RSQHamiltonian<double>>& atomic() const { return this->atomic_parameters; }

    /**
     *  @return the collection of atomic interaction Hamiltonians
     */
    const std::vector<RSQHamiltonian<double>>& interaction() const { return this->interaction_parameters; }

    /**
     *  @return the collection of net atomic Hamiltonians
     */
    const std::vector<RSQHamiltonian<double>>& netAtomic() const { return this->net_atomic_parameters; }

    /**
     *  @return the Hamiltonian of the complete molecule
     */
    const RSQHamiltonian<double>& molecularHamiltonian() const { return this->molecular_hamiltonian; }
};


}  // namespace GQCP
