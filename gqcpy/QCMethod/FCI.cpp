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
#include <pybind11/eigen.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "QCMethod/FCI.hpp"


namespace py = pybind11;


namespace gqcpy {

void bindQCMethodFCI(py::module& module) {
    py::class_<GQCP::QCMethod::FCI>(module, "FCI", "Construct and solve the FCI Hamiltonian.")
        .def(py::init<const std::string, const std::string, const size_t, const size_t>(), py::arg("xyz_filename"), py::arg("basis_set"), py::arg("num_alpha"), py::arg("num_beta"))
        .def("solve", &GQCP::QCMethod::FCI::solve, "Solve the eigenvalue equations such that the lowest energy and corresponding eigenvector become available. ")
        .def("get_energy", &GQCP::QCMethod::FCI::energy, "Get the lowest energy.");
}



}  // namespace gqcpy