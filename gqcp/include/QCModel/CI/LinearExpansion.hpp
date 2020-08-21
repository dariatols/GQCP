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


#include "Basis/ScalarBasis/GTOShell.hpp"
#include "Basis/SpinorBasis/GSpinorBasis.hpp"
#include "Basis/SpinorBasis/RSpinorBasis.hpp"
#include "Basis/SpinorBasis/USpinorBasis.hpp"
#include "Basis/TransformationMatrix.hpp"
#include "Mathematical/Representation/Matrix.hpp"
#include "ONVBasis/SpinResolvedONV.hpp"
#include "ONVBasis/SpinResolvedONVBasis.hpp"
#include "ONVBasis/SpinResolvedSelectedONVBasis.hpp"
#include "Processing/RDM/DOCIRDMBuilder.hpp"
#include "Utilities/aliases.hpp"
#include "Utilities/linalg.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/range/adaptors.hpp>

#include <type_traits>


namespace GQCP {


/**
 *  A class that represents a linear expansion inside an ONV basis.
 * 
 *  @tparam _ONVBasis           the type of ONV basis
 */
template <typename _ONVBasis>
class LinearExpansion {

public:
    using ONVBasis = _ONVBasis;


private:
    ONVBasis onv_basis;              // the ONV basis with respect to which the coefficients are defined
    VectorX<double> m_coefficients;  // the expansion coefficients


public:
    /*
     *  CONSTRUCTORS
     */

    /*
     *  The default constructor.
     */
    LinearExpansion() = default;

    /**
     *  Construct a normalized wave function from possibly non-normalized coefficients
     *
     *  @param onv_basis            the ONV basis with respect to which the coefficients are defined
     *  @param coefficients         the expansion coefficients
     */
    LinearExpansion(const ONVBasis& onv_basis, const VectorX<double>& coefficients) :
        onv_basis {onv_basis},
        m_coefficients {coefficients} {

        if (std::abs(this->m_coefficients.norm() - 1.0) > 1.0e-12) {  // normalize the coefficients if they aren't
            this->m_coefficients.normalize();
        }
    }


    /*
     *  NAMED CONSTRUCTORS
     */

    /**
     *  Create a linear expansion with a normalized coefficient vector (i.e. all the coefficients are equal).
     * 
     *  @param onv_basis            the ONV basis with respect to which the coefficients are defined
     * 
     *  @return a LinearExpansion
     */
    static LinearExpansion<ONVBasis> Constant(const ONVBasis& onv_basis) {

        VectorX<double> coefficients = VectorX<double>::Ones(onv_basis.dimension());
        coefficients.normalize();

        return LinearExpansion<ONVBasis>(onv_basis, coefficients);
    }


    /**
     *  @param GAMESSUS_filename      the name of the GAMESS-US file that contains the spin-resolved selected wave function expansion
     * 
     *  @return the corresponding linear expansion from a given GAMESS-US file
     */
    template <typename Z = ONVBasis>
    static enable_if_t<std::is_same<Z, SpinResolvedSelectedONVBasis>::value, LinearExpansion<Z>> FromGAMESSUS(const std::string& GAMESSUS_filename) {

        // If the filename isn't properly converted into an input file stream, we assume the user supplied a wrong file.
        std::ifstream input_file_stream {GAMESSUS_filename};
        if (!input_file_stream.good()) {
            throw std::runtime_error("LinearExpansionReader(std::string): The provided GAMESS file is illegible. Maybe you specified a wrong path?");
        }

        std::ifstream input_file_stream_count {GAMESSUS_filename};  // made to count the expansion size


        // Do the actual parsing.

        // Read in dummy lines up until we actually get to the ONVs and coefficients.
        std::string line;
        std::string buffer;  // dummy for the counting stream  TODO: find "correcter" way if possible
        while (std::getline(input_file_stream, line)) {
            std::getline(input_file_stream_count, buffer);


            if (line.find("ALPHA") != std::string::npos && line.find("BETA") != std::string::npos && line.find("COEFFICIENT") != std::string::npos) {  // if find() returns an index that's different from the 'not-found' index

                // This line should have dashes and we skip it
                std::getline(input_file_stream, line);
                std::getline(input_file_stream_count, buffer);
                break;
            }
        }

        size_t space_size = 0;
        // Count the number of configurations.
        while (std::getline(input_file_stream_count, buffer)) {
            if (buffer.length() != 0 | buffer[0] != '\n') {
                space_size++;
            }
        }

        VectorX<double> coefficients = VectorX<double>::Zero(space_size);


        std::getline(input_file_stream, line);

        // Read the first line containing the configurations.
        std::vector<std::string> splitted_line;
        boost::split(splitted_line, line, boost::is_any_of("|"));  // split on '|'


        // Create an alpha ONV for the first field
        std::string trimmed_alpha = boost::algorithm::trim_copy(splitted_line[0]);

        // Create a beta ONV for the second field
        std::string trimmed_beta = boost::algorithm::trim_copy(splitted_line[1]);


        size_t index_count = 0;  // counts the number of configurations added
        coefficients(index_count) = std::stod(splitted_line[2]);


        // Parse the trimmed ONV strings into boost::dynamic_bitset to use its functionality.
        std::string reversed_alpha {trimmed_alpha.rbegin(), trimmed_alpha.rend()};
        std::string reversed_beta {trimmed_beta.rbegin(), trimmed_beta.rend()};

        boost::dynamic_bitset<> alpha_transfer {reversed_alpha};
        boost::dynamic_bitset<> beta_transfer {reversed_beta};

        size_t K = alpha_transfer.size();
        size_t N_alpha = alpha_transfer.count();
        size_t N_beta = beta_transfer.count();

        SpinResolvedSelectedONVBasis onv_basis {K, N_alpha, N_beta};
        onv_basis.addONV(reversed_alpha, reversed_beta);


        // Read in the ONVs and the coefficients by splitting the line on '|', and then trimming whitespace.
        // In the GAMESS-US format, the bit strings are ordered in reverse.
        while (std::getline(input_file_stream, line)) {

            index_count++;
            boost::split(splitted_line, line, boost::is_any_of("|"));  // split on '|'


            // Create an alpha SpinUnresolvedONV for the first field
            trimmed_alpha = boost::algorithm::trim_copy(splitted_line[0]);
            if (trimmed_alpha.length() != K) {
                throw std::invalid_argument("LinearExpansionReader(std::string): One of the provided alpha ONVs does not have the correct number of orbitals.");
            }
            reversed_alpha = std::string(trimmed_alpha.rbegin(), trimmed_alpha.rend());

            // Create a beta SpinUnresolvedONV for the second field
            trimmed_beta = boost::algorithm::trim_copy(splitted_line[1]);
            if (trimmed_beta.length() != K) {
                throw std::invalid_argument("LinearExpansionReader(std::string): One of the provided beta ONVs does not have the correct number of orbitals.");
            }
            reversed_beta = std::string(trimmed_beta.rbegin(), trimmed_beta.rend());


            // Create a double for the third field
            coefficients(index_count) = std::stod(splitted_line[2]);
            onv_basis.addONV(reversed_alpha, reversed_beta);

        }  // while getline

        return LinearExpansion<SpinResolvedSelectedONVBasis>(onv_basis, coefficients);
    }


    /**
     *  Create the linear expansion of the given spin-resolved ONV that is expressed in the given USpinorBasis, by projection onto the spin-resolved ONVs expressed with respect to the given RSpinorBasis.
     * 
     *  @param onv                      a spin-resolved ONV expressed with respect to an unrestricted spin-orbital basis
     *  @param r_spinor_basis           the restricted spin-orbital basis that is used to define the resulting linear expansion of ONVs against
     *  @param u_spinor_basis           the unrestricted spin-orbital basis against which the given ONV is expressed
     * 
     *  @return a linear expansion inside a spin-resolved ONV basis
     */
    template <typename Z = ONVBasis>
    static enable_if_t<std::is_same<Z, SpinResolvedONVBasis>::value, LinearExpansion<Z>> FromONVProjection(const SpinResolvedONV& onv, const RSpinorBasis<double, GTOShell>& r_spinor_basis, const USpinorBasis<double, GTOShell>& u_spinor_basis) {


        // Determine the overlap matrices of the underlying scalar orbital bases, which is needed later on.
        auto S = r_spinor_basis.overlap().parameters();                  // the overlap matrix of the restricted MOs/spin-orbitals
        S.basisTransform(r_spinor_basis.coefficientMatrix().inverse());  // now in AO basis

        auto S_alpha = u_spinor_basis.overlap(Spin::alpha).parameters();                  // the overlap matrix of the alpha spin-orbitals
        S_alpha.basisTransform(u_spinor_basis.coefficientMatrix(Spin::alpha).inverse());  // now in AO basis

        auto S_beta = u_spinor_basis.overlap(Spin::beta).parameters();                  // the overlap matrix of the beta spin-orbitals
        S_beta.basisTransform(u_spinor_basis.coefficientMatrix(Spin::beta).inverse());  // now in AO basis

        if (!(S.isApprox(S_alpha, 1.0e-08)) || !(S.isApprox(S_beta, 1.0e-08))) {
            throw std::invalid_argument("LinearExpansion::FromONVProjection(const SpinResolvedONV&, const RSpinorBasis<double, GTOShell>&, const USpinorBasis<double, GTOShell>&): The given spinor bases are not expressed using the same scalar orbital basis.");
        }


        // Prepare some parameters.
        const auto& C = r_spinor_basis.coefficientMatrix();
        const auto& C_alpha = u_spinor_basis.coefficientMatrix(Spin::alpha);
        const auto& C_beta = u_spinor_basis.coefficientMatrix(Spin::beta);


        // Set up the required spin-resolved ONV basis.
        const auto K = onv.numberOfSpatialOrbitals(Spin::alpha);  // assume equal numbers of spin-orbitals for alpha- and beta-electrons
        const auto N_alpha = onv.numberOfElectrons(Spin::alpha);
        const auto N_beta = onv.numberOfElectrons(Spin::beta);
        const SpinResolvedONVBasis onv_basis {K, N_alpha, N_beta};


        // Determine the coefficients through calculating the overlap between two ONVs.
        VectorX<double> coefficients = VectorX<double>::Zero(onv_basis.dimension());

        onv_basis.forEach([&onv, &C_alpha, &C_beta, &C, &S, &coefficients, &onv_basis](const SpinUnresolvedONV& alpha_onv, const size_t I_alpha, const SpinUnresolvedONV& beta_onv, const size_t I_beta) {
            const SpinResolvedONV onv_on {alpha_onv, beta_onv};  // the spin-resolved ONV that should be projected 'on'

            const auto coefficient = onv.calculateProjection(onv_on, C_alpha, C_beta, C, S);
            const auto address = onv_basis.compoundAddress(I_alpha, I_beta);

            coefficients(address) = coefficient;
        });

        return LinearExpansion<Z>(onv_basis, coefficients);
    }


    /**
     *  Create the linear expansion of the given spin-unresolved ONV that is expressed in the given GSpinorBasis, by projection onto the spin-resolved ONVs expressed with respect to another given GSpinorBasis.
     * 
     *  @param onv_of                   a spin-unresolved ONV expressed with respect to an general spinor basis
     *  @param spinor_basis_on          the general spinor basis that is used to define the resulting linear expansion of ONVs against
     *  @param spinor_basis_of          the general spinor basis against which the given ONV is expressed
     * 
     *  @return a linear expansion inside a spin-unresolved ONV basis
     */
    template <typename Z = ONVBasis>
    static enable_if_t<std::is_same<Z, SpinUnresolvedONVBasis>::value, LinearExpansion<Z>> FromONVProjection(const SpinUnresolvedONV& onv_of, const GSpinorBasis<double, GTOShell>& spinor_basis_on, const GSpinorBasis<double, GTOShell>& spinor_basis_of) {


        // Determine the overlap matrices of the underlying scalar orbital bases, which is needed later on.
        auto S_on = spinor_basis_on.overlap().parameters();
        S_on.basisTransform(spinor_basis_on.coefficientMatrix().inverse());  // now in AO basis

        auto S_of = spinor_basis_of.overlap().parameters();
        S_of.basisTransform(spinor_basis_of.coefficientMatrix().inverse());  // now in AO basis

        if (!(S_on.isApprox(S_of, 1.0e-08))) {
            throw std::invalid_argument("LinearExpansion::FromONVProjection(const SpinUnresolvedONV&, const RSpinorBasis<double, GTOShell>&, const GSpinorBasis<double, GTOShell>&): The given spinor bases are not expressed using the same scalar orbital basis.");
        }


        // Prepare some parameters.
        const auto& C_on = spinor_basis_on.coefficientMatrix();
        const auto& C_of = spinor_basis_of.coefficientMatrix();


        // Set up the required spin-resolved ONV basis.
        const auto M = onv_of.numberOfSpinors();
        const auto N = onv_of.numberOfElectrons();
        const SpinUnresolvedONVBasis onv_basis {M, N};


        // Determine the coefficients through calculating the overlap between two ONVs.
        VectorX<double> coefficients = VectorX<double>::Zero(onv_basis.dimension());
        onv_basis.forEach([&onv_of, &C_on, &C_of, &S_on, &coefficients](const SpinUnresolvedONV& onv_on, const size_t I) {
            coefficients(I) = onv_of.calculateProjection(onv_on, C_of, C_on, S_on);
        });

        return LinearExpansion<Z>(onv_basis, coefficients);
    }


    /**
     *  Create a linear expansion that represents the Hartree-Fock wave function.
     * 
     *  @param onv_basis            the ONV basis with respect to which the coefficients are defined
     * 
     *  @return a LinearExpansion
     */
    static LinearExpansion<ONVBasis> HartreeFock(const ONVBasis& onv_basis) {

        VectorX<double> coefficients = VectorX<double>::Unit(onv_basis.dimension(), 0);  // the first ONV in the ONV basis is expected to be the HF determinant

        return LinearExpansion<ONVBasis>(onv_basis, coefficients);
    }


    /**
     *  Create a linear expansion with a random, normalized coefficient vector, with coefficients uniformly distributed in [-1, +1] before any normalization.
     * 
     *  @param onv_basis            the ONV basis with respect to which the coefficients are defined
     * 
     *  @return a LinearExpansion
     */
    static LinearExpansion<ONVBasis> Random(const ONVBasis& onv_basis) {

        VectorX<double> coefficients = VectorX<double>::Random(onv_basis.dimension());
        coefficients.normalize();

        return LinearExpansion<ONVBasis>(onv_basis, coefficients);
    }


    /*
     *  PUBLIC METHODS
     */

    /**
     *  Update the expansion coefficients of this linear expansion so that they correspond to the situation after a transformation of the underlying spinor basis with the given transformation matrix.
     *
     *  @param T            the transformation matrix between the old and the new spinor basis
     * 
     *  @note This method is only available for the full spin-resolved ONV basis.
     *  @note This algorithm was implemented from a description in Helgaker2000.
     */
    template <typename Z = ONVBasis>
    enable_if_t<std::is_same<Z, SpinResolvedONVBasis>::value> basisTransform(const TransformationMatrix<double>& T) {

        const auto K = onv_basis.numberOfOrbitals();  // number of spatial orbitals
        if (K != T.dimension()) {
            throw std::invalid_argument("LinearExpansion::basisTransform(const TransformationMatrix<double>&): The number of spatial orbitals does not match the dimension of the transformation matrix.");
        }


        // LU-decompose the transformation matrix LU decomposition for T
        const auto& lu_decomposition = T.noPivotLUDecompose();

        SquareMatrix<double> L = SquareMatrix<double>::Identity(K, K);
        L.triangularView<Eigen::StrictlyLower>() = lu_decomposition[0];

        SquareMatrix<double> U = SquareMatrix<double>(lu_decomposition[1].triangularView<Eigen::Upper>());
        SquareMatrix<double> U_inv = U.inverse();


        // Calculate t (the operator which allows per-orbital transformation of the wave function)
        SquareMatrix<double> t = SquareMatrix<double>::Identity(K, K) - L + U_inv;


        // Set up spin-unresolved ONV basis variables for the loops over the ONVs
        const SpinUnresolvedONVBasis& alpha_onv_basis = onv_basis.onvBasisAlpha();
        const SpinUnresolvedONVBasis& beta_onv_basis = onv_basis.onvBasisBeta();

        auto dim_alpha = alpha_onv_basis.dimension();
        auto dim_beta = beta_onv_basis.dimension();
        auto N_alpha = alpha_onv_basis.numberOfElectrons();
        auto N_beta = beta_onv_basis.numberOfElectrons();


        /** 
         *  The transformation of the expansion coefficients is adapted from Helgaker2000, chapter 11.9.
         *  For every orbital, a set of correction coefficients will be calculated (Delta C in Helgaker), to update the current coefficients.
         */

        VectorX<double> current_coefficients = this->m_coefficients;  // coefficients will be updated after each orbital transform (C^(n-1)) in Helgaker
        VectorX<double> correction_coefficients = VectorX<double>::Zero(onv_basis.dimension());


        for (size_t m = 0; m < K; m++) {  // iterate over all orbitals

            // Perform alpha and beta CI iterations.

            // 1) Alpha-branch
            SpinUnresolvedONV alpha = alpha_onv_basis.constructONVFromAddress(0);
            for (size_t I_alpha = 0; I_alpha < dim_alpha; I_alpha++) {
                if (!alpha.isOccupied(m)) {
                    for (size_t e1 = 0; e1 < N_alpha; e1++) {    // e1 (electron 1) loops over the (number of) electrons
                        size_t p = alpha.occupationIndexOf(e1);  // retrieve the index of a given electron

                        if (p < m) {
                            size_t address = I_alpha - alpha_onv_basis.vertexWeight(p, e1 + 1);
                            size_t e2 = e1 + 1;
                            size_t q = p + 1;
                            int sign = 1;

                            alpha_onv_basis.shiftUntilNextUnoccupiedOrbital<1>(alpha, address, q, e2, sign);
                            while (q != m) {
                                q++;
                                alpha_onv_basis.shiftUntilNextUnoccupiedOrbital<1>(alpha, address, q, e2, sign);
                            }

                            address += alpha_onv_basis.vertexWeight(q, e2);

                            for (size_t I_beta = 0; I_beta < dim_beta; I_beta++) {
                                correction_coefficients(I_alpha * dim_beta + I_beta) += sign * t(p, m) * current_coefficients(address * dim_beta + I_beta);
                            }
                        }

                        if (p > m) {
                            size_t address = I_alpha - alpha_onv_basis.vertexWeight(p, e1 + 1);
                            size_t e2 = e1 - 1;
                            size_t q = p - 1;
                            int sign = 1;

                            alpha_onv_basis.shiftUntilPreviousUnoccupiedOrbital<1>(alpha, address, q, e2, sign);
                            while (q != m) {
                                q--;
                                alpha_onv_basis.shiftUntilPreviousUnoccupiedOrbital<1>(alpha, address, q, e2, sign);
                            }

                            address += alpha_onv_basis.vertexWeight(q, e2 + 2);
                            for (size_t I_beta = 0; I_beta < dim_beta; I_beta++) {
                                correction_coefficients(I_alpha * dim_beta + I_beta) += sign * t(p, m) * current_coefficients(address * dim_beta + I_beta);
                            }
                        }
                    }

                } else {  // if orbital m is occupied we can perform an in-place operation
                    for (size_t I_beta = 0; I_beta < dim_beta; I_beta++) {
                        correction_coefficients(I_alpha * dim_beta + I_beta) += (t(m, m) - 1) * current_coefficients(I_alpha * dim_beta + I_beta);
                    }
                }


                if (I_alpha < dim_alpha - 1) {  // prevent the last permutation from occurring
                    alpha_onv_basis.transformONVToNextPermutation(alpha);
                }
            }

            current_coefficients += correction_coefficients;
            correction_coefficients.setZero();

            // 2) Beta-branch
            SpinUnresolvedONV beta = beta_onv_basis.constructONVFromAddress(0);

            for (size_t I_beta = 0; I_beta < dim_beta; I_beta++) {
                if (!beta.isOccupied(m)) {
                    for (size_t e1 = 0; e1 < N_beta; e1++) {    // e1 (electron 1) loops over the (number of) electrons
                        size_t p = beta.occupationIndexOf(e1);  // retrieve the index of a given electron

                        if (p < m) {
                            size_t address = I_beta - beta_onv_basis.vertexWeight(p, e1 + 1);
                            size_t e2 = e1 + 1;
                            size_t q = p + 1;
                            int sign = 1;

                            beta_onv_basis.shiftUntilNextUnoccupiedOrbital<1>(beta, address, q, e2, sign);
                            while (q != m) {
                                q++;
                                beta_onv_basis.shiftUntilNextUnoccupiedOrbital<1>(beta, address, q, e2, sign);
                            }

                            address += beta_onv_basis.vertexWeight(q, e2);

                            for (size_t I_alpha = 0; I_alpha < dim_alpha; I_alpha++) {
                                correction_coefficients(I_alpha * dim_beta + I_beta) += sign * t(p, m) * current_coefficients(I_alpha * dim_beta + address);
                            }
                        }

                        if (p > m) {
                            size_t address = I_beta - beta_onv_basis.vertexWeight(p, e1 + 1);
                            size_t e2 = e1 - 1;
                            size_t q = p - 1;

                            int sign = 1;

                            beta_onv_basis.shiftUntilPreviousUnoccupiedOrbital<1>(beta, address, q, e2, sign);
                            while (q != m) {
                                q--;
                                beta_onv_basis.shiftUntilPreviousUnoccupiedOrbital<1>(beta, address, q, e2, sign);
                            }

                            address += beta_onv_basis.vertexWeight(q, e2 + 2);

                            for (size_t I_alpha = 0; I_alpha < dim_alpha; I_alpha++) {
                                correction_coefficients(I_alpha * dim_beta + I_beta) += sign * t(p, m) * current_coefficients(I_alpha * dim_beta + address);
                            }
                        }
                    }

                } else {  // if orbital m is occupied we can perform an in-place operation
                    for (size_t I_alpha = 0; I_alpha < dim_alpha; I_alpha++) {
                        correction_coefficients(I_alpha * dim_beta + I_beta) += (t(m, m) - 1) * current_coefficients(I_alpha * dim_beta + I_beta);
                    }
                }

                if (I_beta < dim_beta - 1) {  // prevent the last permutation from occurring
                    beta_onv_basis.transformONVToNextPermutation(beta);
                }
            }

            current_coefficients += correction_coefficients;
            correction_coefficients.setZero();
        }
        this->m_coefficients = current_coefficients;
    }


    /**
     *  Calculate the one-electron density matrix for a seniority-zero wave function expansion.
     * 
     *  @return the total (spin-summed) 1-DM
     */
    template <typename Z = ONVBasis>
    enable_if_t<std::is_same<Z, SeniorityZeroONVBasis>::value, OneRDM<double>> calculate1DM() const {

        const DOCIRDMBuilder doci_rdm_builder {this->onv_basis};
        return doci_rdm_builder.calculate1RDMs(this->coefficients()).one_rdm;
    }


    /**
     *  @return the Shannon entropy (or information content) of the wave function
     */
    double calculateShannonEntropy() const {

        // Sum over the ONV basis dimension, and only include the term if c_k != 0
        // We might as well replace all coeffients that are 0 by 1, since log(1) = 0 so there is no influence on the final entropy value
        Eigen::ArrayXd coefficients_replaced = this->m_coefficients.unaryExpr([](double c) { return c < 1.0e-18 ? 1 : c; });  // replace 0 by 1

        Eigen::ArrayXd coefficients_squared = coefficients_replaced.square();
        Eigen::ArrayXd log_coefficients_squared = coefficients_squared.log();  // natural logarithm (ln)

        return -1 / std::log(2) * (coefficients_squared * log_coefficients_squared).sum();
    }


    /**
     *  @return the expansion coefficients of this linear expansion wave function model
     */
    const VectorX<double>& coefficients() const { return this->m_coefficients; }


    /**
     *  Iterate over all expansion coefficients and corresponding ONVs, and apply the given callback function.
     * 
     *  @param callback                 the function to be applied in every iteration. Its arguments are an expansion coefficient and the corresponding ONV.
     *  
     */
    template <typename Z = ONVBasis>
    enable_if_t<std::is_same<Z, SpinResolvedONVBasis>::value, void> forEach(const std::function<void(const double, const SpinResolvedONV)>& callback) const {

        // Iterate over all ONVs in this ONV basis, and look up the corresponding coefficient.
        const auto& onv_basis = this->onv_basis;
        const auto& coefficients = this->m_coefficients;
        onv_basis.forEach([&onv_basis, &callback, &coefficients](const SpinUnresolvedONV& onv_alpha, const size_t I_alpha, const SpinUnresolvedONV& onv_beta, const size_t I_beta) {
            const SpinResolvedONV onv {onv_alpha, onv_beta};
            const auto address = onv_basis.compoundAddress(I_alpha, I_beta);
            const auto coefficient = coefficients(address);

            callback(coefficient, onv);
        });
    }


    /** 
     *  @param other            wave function for the comparison
     *  @param tolerance        tolerance for the comparison of coefficients
     * 
     *  @return if two wave functions are equal within a given tolerance
     */
    bool isApprox(const LinearExpansion<ONVBasis>& other, double tolerance = 1e-10) const {

        if (this->onv_basis.dimension() != other.onv_basis.dimension()) {
            return false;
        }

        return areEqualEigenvectors(this->coefficients(), other.coefficients(), tolerance);
    }


    /**
     *  @return the ONV basis that is related to this linear expansion wave function model
     */
    const ONVBasis& onvBasis() const { return onv_basis; }
};


}  // namespace GQCP