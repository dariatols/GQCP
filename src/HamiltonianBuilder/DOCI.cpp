#include "HamiltonianBuilder/DOCI.hpp"


namespace GQCG {


/*
 *  CONSTRUCTORS
 */

/**
 *  Constructor given a @param hamiltonian_parameters and @param fock_space
 */
DOCI::DOCI(FockSpace fock_space) :
    HamiltonianBuilder (),
    fock_space (fock_space),
    dim (fock_space.get_dimension())
{}



/*
 *  OVERRIDDEN PUBLIC METHODS
 */

/**
 *  @return Hamiltonian matrix as an Eigen::MatrixXd given @param hamiltonian_parameters
 */
Eigen::MatrixXd DOCI::constructHamiltonian(const HamiltonianParameters& hamiltonian_parameters) {
    
    auto K = hamiltonian_parameters.get_h().get_dim();
    if (K != this->fock_space.K) {
        throw std::invalid_argument("Basis functions of the Fock space and hamiltonian_parameters are incompatible.");
    }
    
    Eigen::VectorXd diagonal = calculateDiagonal(hamiltonian_parameters);
    Eigen::MatrixXd result_matrix = Eigen::MatrixXd::Zero(this->dim,this->dim);
    // Create the first spin string. Since in DOCI, alpha == beta, we can just treat them as one
    // And multiply all contributions by 2
    ONV onv = this->fock_space.get_ONV(0);  // spin string with address 0

    for (size_t I = 0; I < this->dim; I++) {  // I loops over all the addresses of the spin strings
        // Diagonal contribution
        result_matrix(I, I) += diagonal(I);

        // Off-diagonal contribution
        for (size_t e1 = 0; e1 < this->fock_space.N; e1++) {  // e1 (electron 1) loops over the (number of) electrons
            size_t p = onv.get_occupied_index(e1);  // retrieve the index of the orbital the electron occupies
            for (size_t q = 0; q < p; q++) {  // q loops over SOs
                if (!onv.isOccupied(q)) {  // if q not in I
                    onv.annihilate(p);
                    onv.create(q);

                    size_t J = this->fock_space.getAddress(onv);  // J is the address of a string that couples to I

                    // The loops are p->K and q<p. So, we should normally multiply by a factor 2 (since the summand is symmetric)
                    // However, we are setting both of the symmetric indices of Hamiltonian, so no factor 2 is required
                    result_matrix(I, J) += hamiltonian_parameters.get_g().get(p, q, p, q);
                    result_matrix(J, I) += hamiltonian_parameters.get_g().get(p, q, p, q);

                    onv.annihilate(q);  // reset the spin string after previous creation
                    onv.create(p);  // reset the spin string after previous annihilation

                }  // q < p loop
            }
        }  // p or e1 loop

        // Skip the last permutation
        if (I<this->dim-1) {
            this->fock_space.setNext(onv);
        }

    }  // address (I) loop

    return result_matrix;
}


/**
 *  @return the action of the Hamiltonian (@param hamiltonian_parameters and @param diagonal) on the coefficient vector @param x
 */
Eigen::VectorXd DOCI::matrixVectorProduct(const HamiltonianParameters& hamiltonian_parameters, const Eigen::VectorXd& x, const Eigen::VectorXd& diagonal) {
    auto K = hamiltonian_parameters.get_h().get_dim();
    if (K != this->fock_space.K) {
        throw std::invalid_argument("Basis functions of the Fock space and hamiltonian_parameters are incompatible.");
    }
    // Create the first spin string. Since in DOCI, alpha == beta, we can just treat them as one
    // And multiply all contributions by 2
    ONV onv = this->fock_space.get_ONV(0);  // spin string with address

    // Diagonal contributions
    Eigen::VectorXd matvec = diagonal.cwiseProduct(x);


    // Off-diagonal contributions
    for (size_t I = 0; I < this->dim; I++) {  // I loops over all the addresses of the spin strings
        for (size_t e1 = 0; e1 < this->fock_space.N; e1++) {  // e1 (electron 1) loops over the (number of) electrons
            size_t p = onv.get_occupied_index(e1);  // retrieve the index of the orbital the electron occupies
            for (size_t q = 0; q < p; q++) {  // q loops over SOs
                if (!onv.isOccupied(q)) {  // if q not in I

                    onv.annihilate(p);
                    onv.create(q);

                    size_t J = this->fock_space.getAddress(onv);  // J is the address of a string that couples to I

                    // The loops are p->K and q<p. So, we should normally multiply by a factor 2 (since the summand is symmetric)
                    // However, we are setting both of the symmetric indices of Hamiltonian, so no factor 2 is required
                    matvec(I) += hamiltonian_parameters.get_g().get(p, q, p, q) * x(J);
                    matvec(J) += hamiltonian_parameters.get_g().get(p, q, p, q) * x(I);

                    onv.annihilate(q);  // reset the spin string after previous creation
                    onv.create(p);  // reset the spin string after previous annihilation

                }  // q < p loop
            }
        }  // p or e1 loop

        // Skip the last permutation
        if (I<this->dim-1) {
            this->fock_space.setNext(onv);
        }

    }  // address (I) loop

    return matvec;
}


/**
 *  @return the diagonal of the matrix representation of the Hamiltonian given @param hamiltonian_parameters
 */
Eigen::VectorXd DOCI::calculateDiagonal(const HamiltonianParameters& hamiltonian_parameters) {

    Eigen::VectorXd diagonal = Eigen::VectorXd::Zero(this->dim);
    // Create the first spin string. Since in DOCI, alpha == beta, we can just treat them as one
    // And multiply all contributions by 2
    ONV onv = this->fock_space.get_ONV(0);  // onv with address 0

    for (size_t I = 0; I < this->dim; I++) {  // I loops over addresses of spin strings
        for (size_t e1 = 0; e1 < this->fock_space.N; e1++) {  // e1 (electron 1) loops over the (number of) electrons
            size_t p = onv.get_occupied_index(e1);  // retrieve the index of the orbital the electron occupies

            diagonal(I) += 2 * hamiltonian_parameters.get_h().get(p,p) + hamiltonian_parameters.get_g().get(p,p,p,p);
            for (size_t e2 = 0; e2 < e1; e2++) {  // e2 (electron 2) loops over the (number of) electrons
                // Since we are doing a restricted summation q<p (and thus e2<e1), we should multiply by 2 since the summand argument is symmetric.
                size_t q = onv.get_occupied_index(e2);  // retrieve the index of the orbital the electron occupies
                diagonal(I) += 2 * (2*hamiltonian_parameters.get_g().get(p,p,q,q) - hamiltonian_parameters.get_g().get(p,q,q,p));
            }  // q or e2 loop
        } // p or e1 loop

        // Skip the last permutation
        if (I<this->dim-1) {
            this->fock_space.setNext(onv);
        }

    }  // address (I) loop
    return diagonal;
}



}  // namespace GQCG