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

#ifndef __VOTCA_CTP_GSL_H
#define __VOTCA_CTP_GSL_H

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/symmetric.hpp>
#include <boost/numeric/ublas/io.hpp>

#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_eigen.h>

#include <boost/numeric/conversion/cast.hpp>

#define DEBUG_LINALG
#ifdef DEBUG_LINALG
#include <chrono>
#endif


using namespace std;
namespace ub = boost::numeric::ublas;


namespace votca { namespace ctp { namespace linalg {

    /**
     * \brief inverts A
     * @param A symmetric positive definite matrix
     * @param V inverse matrix
     */    
inline void invert_symm( const ub::matrix<double> &A, ub::matrix<double> &V){
        // matrix inversion using gsl

    #ifdef DEBUG_LINALG
        auto start = std::chrono::system_clock::now();
    #endif
    
        gsl_error_handler_t *handler = gsl_set_error_handler_off();
	const size_t N = A.size1();
	// signum s (for LU decomposition)
	int s;
        //make copy of A as A is destroyed by GSL
        ub::matrix<double> work=A;
        V.resize(N, N, false);
        
	// Define all the used matrices
        gsl_matrix_view A_view = gsl_matrix_view_array(&work(0,0), N, N);
        gsl_matrix_view V_view = gsl_matrix_view_array(&V(0,0), N, N);
	gsl_permutation * perm = gsl_permutation_alloc (N);
        
	// Make LU decomposition of matrix A_view
	gsl_linalg_LU_decomp (&A_view.matrix, perm, &s);

	// Invert the matrix A_view
	(void)gsl_linalg_LU_invert (&A_view.matrix, perm, &V_view.matrix);

        gsl_set_error_handler(handler);

    #ifdef DEBUG_LINALG
        auto end = std::chrono::system_clock::now();

        std::chrono::duration<double> elapsed_seconds = end - start;

        std::cout << "\n... ... ... \x1b[0;34mGSL Invert" 
                  << "[" << A.size1() << "x" << A.size2() << "]"
                  << " time: " << elapsed_seconds.count() 
                  << "s\x1b[0;39m";
    #endif       
        
}

/**
 * \brief eigenvalues of a symmetric matrix A*x=E*x
 * @param E vector of eigenvalues
 * @param V input: matrix to diagonalize
 * @param V output: eigenvectors      
 * 
 * This function wraps gsl_eigen_symmv / DSYEV
 * 
 */
inline void eigenvalues_symm(const ub::matrix<double> &A, 
                                     ub::vector<double> &E, 
                                     ub::matrix<double> &V)
{
    
   #ifdef DEBUG_LINALG
        auto start = std::chrono::system_clock::now();
    #endif
    
	gsl_error_handler_t *handler = gsl_set_error_handler_off();
	const size_t N = A.size1();
        
        // gsl does not handle conversion of a symmetric_matrix 
        ub::matrix<double> _A( N,N );
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
	//return (status == 0);
        
    #ifdef DEBUG_LINALG
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        std::cout << "\n... ... ... \x1b[0;34mGSL Eigenvalues" 
                  << "[" << A.size1() << "x" << A.size2() << "]"
                  << " time: " << elapsed_seconds.count() 
                  << "s\x1b[0;39m";
    #endif       
        
};

}}}

/*
 * Since ublas has very slow prod() function, we overwrite it here 
 * by the gsl (blas) product function. GSL has separate implementations 
 * for floats and doubles hence we have two specializations for these types
 * Separate templates are provided for diagonal and symmetric matreces
 * and their combinations. To check  whether a specific routine is called, 
 * define the GSLDEBUG flag
 */
namespace boost { namespace numeric { namespace ublas {

// specialization for double precision (dgemm)
template<class F, class A>
inline matrix<double,F,A>    // prod( m1, m2 )
prod(const matrix<double,F,A> &m1, const matrix<double,F,A> &m2)
{
        
    #ifdef DEBUG_LINALG
        auto start = std::chrono::system_clock::now();
    #endif
          
       gsl_matrix_const_view mA = gsl_matrix_const_view_array (&m1(0,0), m1.size1(), m1.size2());
       gsl_matrix_const_view mB = gsl_matrix_const_view_array (&m2(0,0), m2.size1(), m2.size2());

       boost::numeric::ublas::matrix<double,F,A> AxB( m1.size1(), m2.size2() );
       gsl_matrix_view mC = gsl_matrix_view_array (&AxB(0,0), AxB.size1(), AxB.size2());

       int status = gsl_blas_dgemm (CblasNoTrans, CblasNoTrans,
                  1.0, &mA.matrix, &mB.matrix,
                  0.0, &mC.matrix);
       assert(status == 0);

    #ifdef DEBUG_LINALG
        auto end = std::chrono::system_clock::now();

        std::chrono::duration<double> elapsed_seconds = end - start;

        std::cout << "\n... ... ... \x1b[0;34mGSL " 
                  << "[" << m1.size1() << "x" << m1.size2() << "]" 
                  << "*[" << m2.size1() << "x" << m2.size2() << "]"
                  << " time: " << elapsed_seconds.count() 
                  << "s\x1b[0;39m";
    #endif       
       
    return AxB;
       
}    
    
    // specialization for single precision (sgemm)
    template<class F, class A>
    inline matrix<float,F,A>    // prod( m1, m2 )
    prod(const matrix<float,F,A> &m1, const matrix<float,F,A> &m2)
    {
    #ifdef DEBUG_LINALG
        auto start = std::chrono::system_clock::now();
    #endif
          
       gsl_matrix_float_const_view mA = gsl_matrix_float_const_view_array (&m1(0,0), m1.size1(), m1.size2());
       gsl_matrix_float_const_view mB = gsl_matrix_float_const_view_array (&m2(0,0), m2.size1(), m2.size2());

       boost::numeric::ublas::matrix<float,F,A> AxB( m1.size1(), m2.size2() );
       gsl_matrix_float_view mC = gsl_matrix_float_view_array (&AxB(0,0), AxB.size1(), AxB.size2());

       gsl_blas_sgemm (CblasNoTrans, CblasNoTrans,
                  1.0, &mA.matrix, &mB.matrix,
                  0.0, &mC.matrix);

    #ifdef DEBUG_LINALG
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        std::cout << "\n... ... ... \x1b[0;34mGSL " 
                  << "[" << m1.size1() << "x" << m1.size2() << "]" 
                  << "*[" << m2.size1() << "x" << m2.size2() << "]"
                  << " time: " << elapsed_seconds.count() 
                  << "s\x1b[0;39m";
    #endif       
       

       return AxB;
    }        
    
}}}

#endif  // __VOTCA_CTP_GSL_H
