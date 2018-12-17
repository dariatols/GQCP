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
#define BOOST_TEST_MODULE "elements"

#include <boost/test/unit_test.hpp>
#include <boost/test/included/unit_test.hpp>  // include this to get main(), otherwise the compiler will complain

#include "elements.hpp"



BOOST_AUTO_TEST_CASE ( elementToAtomicNumber ) {
    
    BOOST_CHECK_EQUAL(GQCP::elements::elementToAtomicNumber("H"), 1);
    BOOST_CHECK_EQUAL(GQCP::elements::elementToAtomicNumber("C"), 6);
    BOOST_CHECK_EQUAL(GQCP::elements::elementToAtomicNumber("Mg"), 12);
}


BOOST_AUTO_TEST_CASE ( atomicNumberToElement ) {

    BOOST_CHECK_EQUAL(GQCP::elements::atomicNumberToElement(1), "H");
    BOOST_CHECK_EQUAL(GQCP::elements::atomicNumberToElement(6), "C");
    BOOST_CHECK_EQUAL(GQCP::elements::atomicNumberToElement(12), "Mg");
}
