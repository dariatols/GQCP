// This file is part of GQCG-gqcp.
// 
// Copyright (C) 2017-2018  the GQCG developers
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
#ifndef GQCP_LIBINTCOMMUNICATOR_HPP
#define GQCP_LIBINTCOMMUNICATOR_HPP


#include "AOBasis.hpp"
#include "Molecule.hpp"
#include "Operator/OneElectronOperator.hpp"
#include "Operator/TwoElectronOperator.hpp"

#include <Eigen/Dense>
#include <unsupported/Eigen/CXX11/Tensor>
#include <libint2.hpp>


namespace GQCP {


/**
 *  A singleton class that takes care of interfacing with the Libint2 (version >2.2.0) C++ API
 *
 *  Singleton class template from (https://stackoverflow.com/a/1008289)
 */
class LibintCommunicator {
private:
    /**
     *  Private constructor as required by the singleton class design
     */
    LibintCommunicator();

    /**
     *  Private destructor as required by the singleton class design
     */
    ~LibintCommunicator();

public:
    /**
     *  @return the static singleton instance
     */
    static LibintCommunicator& get();


    /**
     *  Remove the public copy constructor and a public assignment operator
     */
    LibintCommunicator(LibintCommunicator const& libint_communicator) = delete;
    void operator=(LibintCommunicator const& libint_communicator) = delete;


    // PUBLIC METHODS
    /**
     *  @return a std::vector<libint2::Atom> based on a given std::vector<GQCP::Atom> @param atoms
     */
    std::vector<libint2::Atom> interface(const std::vector<GQCP::Atom>& atoms) const;


    /**
     *  @return the OneElectronOperator corresponding to the matrix representation of @param operator_type in the given
     *  @param ao_basis. The corresponding @param molecule is also given as an argument, to be able to access
     */
    GQCP::OneElectronOperator calculateOneElectronIntegrals(libint2::Operator operator_type, const GQCP::AOBasis& ao_basis) const;

    /**
     *  @return the TwoElectronOperator corresponding to the matrix representation of @param operator_type in the given
     *  @param ao_basis
     */
    GQCP::TwoElectronOperator calculateTwoElectronIntegrals(libint2::Operator operator_type, const GQCP::AOBasis& ao_basis) const;
};


}  // namespace GQCP


#endif  // GQCP_LIBINTCOMMUNICATOR_HPP
