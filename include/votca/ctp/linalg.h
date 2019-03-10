/* 
 * Copyright 2009-2018 The VOTCA-MPIP Development Team (http://www.votca.org)
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

#ifndef __VOTCA_CTP_LINALG_H
#define	__VOTCA_CTP_LINALG_H

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/operation.hpp>
#include <boost/numeric/ublas/banded.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>

#include <votca/ctp/votca_config.h>

#ifdef GSL
#include <votca/ctp/gsl.h>  
#else
#include <votca/ctp/eigen.h>  
#endif

#ifdef DEBUG_LINALG
#include <chrono>
#endif

namespace votca { namespace ctp {
    namespace ub = boost::numeric::ublas;

    /**
     * \brief inverts A
     * @param A symmetric positive definite matrix
     * @param V inverse matrix
     */
    void linalg_invert( const boost::numeric::ublas::matrix<double> &A, 
                              boost::numeric::ublas::matrix<double> &V 
    );
 
     /**
     * \brief eigenvalues of a symmetric matrix A*x=E*x
     * @param E vector of eigenvalues
     * @param V input: matrix to diagonalize
     * @param V output: eigenvectors
     */
    void linalg_eigenvalues(const boost::numeric::ublas::matrix<double> &A, 
                                  boost::numeric::ublas::vector<double> &E, 
                                  boost::numeric::ublas::matrix<double> &V 
    );
}}        


namespace boost { namespace numeric { namespace ublas {

    /**
    * \brief product of two matreces
    * @param m1 matrix
    * @param m2 matrix
    * @param return m1*m2 
    * 
    * overwrites the ublas version either with Eigen or GSL products 
    */
    template<class T, class F, class A>
    inline boost::numeric::ublas::matrix<T,F,A>
    prod(   const boost::numeric::ublas::matrix<T,F,A> &m1, 
            const boost::numeric::ublas::matrix<T,F,A> &m2) {

#ifdef DEBUG_LINALG
    auto start = std::chrono::system_clock::now();
#endif
       return prod(m1,m2);
#ifdef DEBUG_LINALG
    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);

    std::cout << " elapsed time: " << elapsed_seconds.count() << "s\n";
#endif       
    }    
   
}}}



#endif	/* __VOTCA_CTP_LINALG_H */

