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
#include "Basis/AOBasis.hpp"

#include "Basis/GTOBasisSet.hpp"
#include "Basis/Integrals/Interfaces/LibintInterfacer.hpp"
#include "Basis/Integrals/Interfaces/LibcintInterfacer.hpp"
#include "Basis/Integrals/IntegralCalculator.hpp"
#include "Basis/Integrals/IntegralEngine.hpp"


namespace GQCP {


/*
 *  CONSTRUCTORS
 */

/**
 *  @param shell_set        the set of shells that are placed on the nuclei
 */
AOBasis::AOBasis(const ShellSet<GTOShell>& shell_set) :
    shell_set (shell_set)
{}


/**
 *  Construct an AO basis by placing shells corresponding to the basisset specification on every nucleus of the molecule
 *
 *  @param molecule             the molecule containing the nuclei on which the shells should be centered
 *  @param basisset_name        the name of the basisset, e.g. "STO-3G"
 *
 *  Note that the normalization factors of the spherical (or axis-aligned Cartesian) GTO primitives are embedded in the contraction coefficients of the underlying shells
 */
AOBasis::AOBasis(const Molecule& molecule, const std::string& basisset_name) :
    AOBasis(GTOBasisSet(basisset_name).generate(molecule))
{
    this->shell_set.embedNormalizationFactorsOfPrimitives();
}



/*
 *  PUBLIC METHODS
 */

/**
 *  @return the number of basis functions in this AO basis
 */
size_t AOBasis::numberOfBasisFunctions() const {
    return this->shell_set.numberOfBasisFunctions();
}



/*
 *  PUBLIC METHODS - LIBINT2 INTEGRALS
 */

/**
 *  @return the matrix representation of the overlap operator in this AO basis
 */
ChemicalMatrix<double> AOBasis::calculateLibintOverlapIntegrals() const {

    // Construct the libint engine
    const auto max_nprim = this->shell_set.maximumNumberOfPrimitives();
    const auto max_l = this->shell_set.maximumAngularMomentum();
    auto engine = IntegralEngine::Libint(Operator::Overlap(), max_nprim, max_l);  // cannot be const because libint2::Engine::compute() is not a const method


    // Calculate the integrals using the engine
    const auto integrals = IntegralCalculator::calculate(engine, this->shell_set);
    return integrals[0];
}


/**
 *  @return the matrix representation of the kinetic energy operator in this AO basis
 */
ChemicalMatrix<double> AOBasis::calculateLibintKineticIntegrals() const {

    // Construct the libint engine
    const auto max_nprim = this->shell_set.maximumNumberOfPrimitives();
    const auto max_l = this->shell_set.maximumAngularMomentum();
    auto engine = IntegralEngine::Libint(Operator::Kinetic(), max_nprim, max_l);  // cannot be const because libint2::Engine::compute() is not a const method


    // Calculate the integrals using the engine
    const auto integrals = IntegralCalculator::calculate(engine, this->shell_set);
    return integrals[0];
}


/**
 *  @return the matrix representation of the nuclear attraction operator in this AO basis
 */
ChemicalMatrix<double> AOBasis::calculateLibintNuclearIntegrals() const {

    // Construct the libint engine
    const auto max_nprim = this->shell_set.maximumNumberOfPrimitives();
    const auto max_l = this->shell_set.maximumAngularMomentum();
    auto engine = IntegralEngine::Libint(Operator::NuclearAttraction(this->shell_set.nuclei()), max_nprim, max_l);  // cannot be const because libint2::Engine::compute() is not a const method


    // Calculate the integrals using the engine
    const auto integrals = IntegralCalculator::calculate(engine, this->shell_set);
    return integrals[0];
}


/**
 *  @param origin       the origin of the dipole
 *
 *  @return the matrix representation of the Cartesian components of the electrical dipole operator in this AO basis
 */
std::array<ChemicalMatrix<double>, 3> AOBasis::calculateLibintDipoleIntegrals(const Vector<double, 3>& origin) const {

    // Construct the libint engine
    const auto max_nprim = this->shell_set.maximumNumberOfPrimitives();
    const auto max_l = this->shell_set.maximumAngularMomentum();
    auto engine = IntegralEngine::Libint(Operator::ElectronicDipole(origin), max_nprim, max_l);  // cannot be const because libint2::Engine::compute() is not a const method


    // Calculate the integrals using the engine
    const auto all_integrals = IntegralCalculator::calculate(engine, this->shell_set);
    return {all_integrals[0], all_integrals[1], all_integrals[2]};
}


/**
 *  @return the matrix representation of the Coulomb repulsion operator in this AO basis
 */
ChemicalRankFourTensor<double> AOBasis::calculateLibintCoulombRepulsionIntegrals() const {

    // Construct the libint engine
    const auto max_nprim = this->shell_set.maximumNumberOfPrimitives();
    const auto max_l = this->shell_set.maximumAngularMomentum();
    auto engine = IntegralEngine::Libint(Operator::Coulomb(), max_nprim, max_l);  // cannot be const because libint2::Engine::compute() is not a const method


    // Calculate the integrals using the engine
    const auto integrals = IntegralCalculator::calculate(engine, this->shell_set);
    return integrals[0];
}



/*
 *  PUBLIC METHODS - LIBCINT INTEGRALS
 *  Note that the Libcint integrals should only be used for Cartesian ShellSets
 */

/**
 *  Calculate the overlap integrals using Libcint: only use this for all-Cartesian ShellSets
 *
 *  @return the matrix representation of the overlap operator in this AO basis, using the libcint integral engine
 */
ChemicalMatrix<double> AOBasis::calculateLibcintOverlapIntegrals() const {

    auto engine = IntegralEngine::Libcint(Operator::Overlap(), this->shell_set);  // cannot be const: Libint2 has a non-const compute() method inside its interface
    const auto integrals = IntegralCalculator::calculate(engine, this->shell_set);
    return integrals[0];
}


/**
 *  Calculate the kinetic energy integrals using Libcint: only use this for all-Cartesian ShellSets
 *
 *  @return the matrix representation of the kinetic energy operator in this AO basis, using the libcint integral engine
 */
ChemicalMatrix<double> AOBasis::calculateLibcintKineticIntegrals() const {

    auto engine = IntegralEngine::Libcint(Operator::Kinetic(), this->shell_set);  // cannot be const: Libint2 has a non-const compute() method inside its interface
    const auto integrals = IntegralCalculator::calculate(engine, this->shell_set);
    return integrals[0];
}


/**
 *  Calculate the nuclear attraction energy integrals using Libcint: only use this for all-Cartesian ShellSets
 *
 *  @return the matrix representation of the nuclear attraction operator in this AO basis, using the libcint integral engine
 */
ChemicalMatrix<double> AOBasis::calculateLibcintNuclearIntegrals() const {

    auto engine = IntegralEngine::Libcint(Operator::NuclearAttraction(this->shell_set.nuclei()), this->shell_set);  // cannot be const: Libint2 has a non-const compute() method inside its interface
    const auto integrals = IntegralCalculator::calculate(engine, this->shell_set);
    return integrals[0];
}


/**
 *  Calculate the electrical dipole integrals using Libcint: only use this for all-Cartesian ShellSets
 *
 *  @param origin       the origin of the dipole
 *
 *  @return the matrix representation of the Cartesian components of the electrical dipole operator in this AO basis, using the libcint integral engine
 */
std::array<ChemicalMatrix<double>, 3> AOBasis::calculateLibcintDipoleIntegrals(const Vector<double, 3>& origin) const {

    auto engine = IntegralEngine::Libcint(Operator::ElectronicDipole(origin), this->shell_set);  // cannot be const: Libint2 has a non-const compute() method inside its interface
    const auto integrals = IntegralCalculator::calculate(engine, this->shell_set);
    return {integrals[0], integrals[1], integrals[2]};
}


/**
 *  Calculate the Coulomb repulsion energy integrals using Libcint: only use this for all-Cartesian ShellSets
 *
 *  @return the matrix representation of the Coulomb repulsion operator in this AO basis, using the libcint integral engine
 */
ChemicalRankFourTensor<double> AOBasis::calculateLibcintCoulombRepulsionIntegrals() const {

    auto engine = IntegralEngine::Libcint(Operator::Coulomb(), this->shell_set);  // cannot be const: Libint2 has a non-const compute() method inside its interface
    const auto integrals = IntegralCalculator::calculate(engine, this->shell_set);
    return integrals[0];
}


}  // namespace GQCP
