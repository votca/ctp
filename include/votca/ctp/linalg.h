/* 
 * Copyright 2009-2018 The VOTCA Development Team (http://www.votca.org)
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
 */

#ifndef __VOTCA_CTP_LINALG_H
#define	__VOTCA_CTP_LINALG_H

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/operation.hpp>
#include <boost/numeric/ublas/banded.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>

#if GSL
#include <votca/ctp/gsl.h>  
#else
#include <votca/ctp/eigen.h>  
#endif

namespace votca { namespace ctp {
    namespace ub = boost::numeric::ublas;

    /**
     * \brief inverts A
     * @param A symmetric positive definite matrix
     * @param V inverse matrix
     *
     * This function wraps the inversion of a matrix
     */
    void linalg_invert( const ub::matrix<double> &A, ub::matrix<double> &V );
 
     /**
     * \brief eigenvalues of a symmetric matrix A*x=E*x
     * @param E vector of eigenvalues
     * @param V input: matrix to diagonalize
     * @param V output: eigenvectors      
     * 
     * This function wraps gsl_eigen_symmv / DSYEV
     * 
     */
    bool linalg_eigenvalues(const ub::matrix<double> &A, ub::vector<double> &E, ub::matrix<double> &V );
}}        

/**
 * \brief product of two matreces
 * @param m1 matrix
 * @param m2 matrix
 * @param return m1*m2 
 * 
 * overwrites the ublas version either with Eigen or GSL products 
 */
namespace boost { namespace numeric { namespace ublas {
    namespace ub = boost::numeric::ublas;

    template<class T, class F, class A>
    inline ub::matrix<T,F,A>
    prod(const ub::matrix<T,F,A> &m1, const ub::matrix<T,F,A> &m2)
    {
       return prod(m1,m2);
    }    
   
}}}



#endif	/* __VOTCA_CTP_LINALG_H */

