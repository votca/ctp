/*
 * Copyright 2009-2019 The VOTCA-MPIP Development Team (http://www.votca.org)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * author: Denis Andrienko
 */

#ifndef __VOTCA_CTP_EIGEN_H
#define __VOTCA_CTP_EIGEN_H

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/symmetric.hpp>
#include <boost/numeric/ublas/io.hpp>

#include <boost/numeric/conversion/cast.hpp>

#include <votca/tools/eigen.h>
#include <Eigen/Eigenvalues>

/*
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_eigen.h>
*/

#ifdef DEBUG_LINALG
#include <chrono>
#endif


namespace boost { namespace numeric { namespace ublas {

// specialization for double precision
template<class F, class A>
inline matrix<double,F,A>    // prod( m1, m2 )
prod(const matrix<double,F,A> &m1, const matrix<double,F,A> &m2)
{

    #ifdef DEBUG_LINALG
    auto start = std::chrono::system_clock::now();
    #endif

    boost::numeric::ublas::matrix<double,F,A> AxB( m1.size1(), m2.size2() );

    // Note that a Map<Type> m(&data) gives a view into the data (can change it)
    // while assignments Matrix<Type> m = Map< Matrix<Type> > (&data)
    // lead to a data copy and lost connection between the reference and matrix
    Eigen::Map<const Eigen::MatrixXd> mA_( &(m1.data()[0]), m1.size2(), m1.size1() );
    Eigen::Map<const Eigen::MatrixXd> mB_( &(m2.data()[0]), m2.size2(), m2.size1() );
    Eigen::Map<Eigen::MatrixXd> AxB_( &(AxB.data()[0]), AxB.size2(), AxB.size1() );

    AxB_ = mB_*mA_;

    #ifdef DEBUG_LINALG
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "\n... ... ... \x1b[0;34mEigen "
              << "[" << m1.size1() << "x" << m1.size2() << "]"
              << "*[" << m2.size1() << "x" << m2.size2() << "]"
              << " time: " << elapsed_seconds.count()
              << "s\x1b[0;39m";
    #endif

    return AxB;

            }

// specialization for single precision
template<class F, class A>
inline matrix<float, F, A> // prod( m1, m2 )
prod(const matrix<float, F, A> &m1, const matrix<float, F, A> &m2) {

    #ifdef DEBUG_LINALG
    auto start = std::chrono::system_clock::now();
    #endif

    boost::numeric::ublas::matrix<float, F, A> AxB(m1.size1(), m2.size2());

    Eigen::Map<const Eigen::MatrixXf> mA_(&(m1.data()[0]), m1.size2(), m1.size1());
    Eigen::Map<const Eigen::MatrixXf> mB_(&(m2.data()[0]), m2.size2(), m2.size1());
    Eigen::Map<Eigen::MatrixXf> AxB_(&(AxB.data()[0]), AxB.size2(), AxB.size1());

    AxB_ = mB_*mA_;

    #ifdef DEBUG_LINALG
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "\n... ... ... \x1b[0;34mEigen "
            << "[" << m1.size1() << "x" << m1.size2() << "]"
            << "*[" << m2.size1() << "x" << m2.size2() << "]"
            << " time: " << elapsed_seconds.count()
            << "s\x1b[0;39m";
    #endif

    return AxB;
}

}}}

namespace votca { namespace ctp { namespace linalg {

inline void invert_symm( const ub::matrix<double> &A, ub::matrix<double> &V){
    throw "Inversion of a matrix, linalg_invert, is not implemented in eigen.h";
}

/**
 * \brief eigenvalues of a symmetric matrix A*x=E*x
 * @param E vector of eigenvalues
 * @param V input: matrix to diagonalize
 * @param V output: eigenvectors
 *
 * This function wraps Eigen::EigenSolver
 *
 */
inline void eigenvalues_symm(const ub::matrix<double> &A,
                                   ub::vector<double> &E,
                                   ub::matrix<double> &V)
{

    #ifdef DEBUG_LINALG
        auto start = std::chrono::system_clock::now();
    #endif

/*
        gsl_error_handler_t *handler = gsl_set_error_handler_off();
	const size_t N = A.size1();


        V.resize(N, N, false);
        
        // gsl does not handle conversion of a symmetric_matrix 
        ub::matrix<double> _A( N,N );
        //make copy of A as A is destroyed by GSL
        _A = A;
        
	E.resize(N, false);
	V.resize(N, N, false);
	gsl_matrix_view A_view = gsl_matrix_view_array(&_A(0,0), N, N);
	gsl_vector_view E_view = gsl_vector_view_array(&E(0), N);
	gsl_matrix_view V_view = gsl_matrix_view_array(&V(0,0), N, N);
	gsl_eigen_symmv_workspace *w = gsl_eigen_symmv_alloc(N);

	int status = gsl_eigen_symmv(&A_view.matrix, &E_view.vector, &V_view.matrix, w);
        assert(status == 0);
	gsl_eigen_symmv_sort(&E_view.vector, &V_view.matrix, GSL_EIGEN_SORT_VAL_ASC);
	gsl_eigen_symmv_free(w);
	gsl_set_error_handler(handler);   
        
        std::cout << "\nREFERENCE";
        std::cout << "\n A" << A;
        std::cout << "\n E" << E;
        std::cout << "\n V" << V;
        std::cout << "\nREFERENCE\n";
        
*/        
    Eigen::Map<const Eigen::MatrixXd> A_( &(A.data()[0]), A.size2(), A.size1() );

    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> es(A_.transpose());
    /*
      std::cout << "\nEIGEN";
      std::cout << "\n E\n" << es.eigenvalues();
      std::cout << "\n V\n" << es.eigenvectors();
      std::cout << "\nEIGEN";
    */
    
    // copy back to UBLAS objects
    Eigen::Map<Eigen::VectorXd>( &E(0), A.size2() ) = es.eigenvalues();
    Eigen::Map<Eigen::MatrixXd>( &V(0,0), A.size2(), A.size1() ) = es.eigenvectors().transpose();
        
    #ifdef DEBUG_LINALG
        auto end = std::chrono::system_clock::now();

        std::chrono::duration<double> elapsed_seconds = end - start;

        std::cout << "\n... ... ... \x1b[0;34mEigen Eigenvalues"
                  << "[" << A.size1() << "x" << A.size2() << "]"
                  << " time: " << elapsed_seconds.count()
                  << "s\x1b[0;39m";
    #endif
}

}}}


#endif  // __VOTCA_CTP_EIGEN_H
