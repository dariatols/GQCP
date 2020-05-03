// / RHF / DiagonalRHFFockMatrixObjective
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

#include "QCMethod/HF/UHF/UHF.hpp"

#include "Mathematical/Algorithm/IterativeAlgorithm.hpp"
#include "QCMethod/HF/UHF/UHFSCFSolver.hpp"

#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace gqcpy {


void bindQCMethodUHF(py::module& module) {
    py::class_<GQCP::QCMethod::UHF<double>>(module, "UHF", "The unrestricted Hartree-Fock quantum chemical method.")

        .def_static(
            "optimize",
            [](GQCP::IterativeAlgorithm<GQCP::UHFSCFEnvironment<double>>& solver, GQCP::UHFSCFEnvironment<double>& environment) {
                return GQCP::QCMethod::UHF<double>().optimize(solver, environment);
            },
            py::arg("solver"),
            py::arg("environment"),
            "Optimize the UHF wave function model.");
}


}  // namespace gqcpy
