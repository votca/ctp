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

namespace votca { namespace ctp { namespace linalg {
    namespace ub = boost::numeric::ublas;

    /**
     * \brief inverts A
     * @param A symmetric positive definite matrix
     * @param V inverse matrix
     */
    void invert_symm( const boost::numeric::ublas::matrix<double> &A, 
                              boost::numeric::ublas::matrix<double> &V 
    );
 
     /**
     * \brief eigenvalues of a symmetric matrix A*x=E*x
     * @param E vector of eigenvalues
     * @param V input: matrix to diagonalize
     * @param V output: eigenvectors
     */
    void eigenvalues_symm(const boost::numeric::ublas::matrix<double> &A, 
                                boost::numeric::ublas::vector<double> &E, 
                                boost::numeric::ublas::matrix<double> &V 
    );
}}}   

/* boost has extremely slow linear algebra operations
 * as a workaround, we override prod and other functions
 * with function provided by gsl or eigen 
 */
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
        return prod(m1,m2);
    }    


/// transpose products
template<class T, class F, class A>
inline matrix<T,F,A>    // prod( trans(m1), m2 )
prod(const matrix_unary2<matrix<T,F,A>,scalar_identity<T> > &m1, const matrix<T,F,A> &m2)
{
    #ifdef DEBUG_LINALG 
      std::cout << "\n... ... ... \x1b[0;33mRedirection of [transpose mat, mat] to [mat, mat]\x1b[0;39m";
    #endif
   boost::numeric::ublas::matrix<T,F,A> _m1 = m1;
   return prod(_m1,m2);
}

template<class T, class F, class A>
inline matrix<T,F,A>    // prod( m1, trans(m2) )
prod(const matrix<T,F,A> &m1, const matrix_unary2<matrix<T,F,A>,scalar_identity<T> > &m2)
{
    #ifdef DEBUG_LINALG 
      std::cout << "\n... ... ... \x1b[0;33mRedirection of [mat, transpose mat] to [mat, mat]\x1b[0;39m";
    #endif

   const boost::numeric::ublas::matrix<T,F,A> _m2 = m2;
   return prod(m1,_m2);
}

template<class T, class F, class A>
inline matrix<T,F,A>    // prod( trans(m1), trans(m2) )
prod(const matrix_unary2<matrix<T,F,A>,scalar_identity<T> > &m1, const matrix_unary2<matrix<T,F,A>,scalar_identity<T> > &m2)
{
    #ifdef DEBUG_LINALG 
      std::cout << "\n... ... ... \x1b[0;33mRedirection of [transpose mat, transpose mat] to [mat, mat]\x1b[0;39m";
    #endif


   boost::numeric::ublas::matrix<T,F,A> _m1 = m1;
   boost::numeric::ublas::matrix<T,F,A> _m2 = m2;

   return prod(_m1,_m2);
}

/// diagonal matrix
template<class T, class F, class L, class A>
inline matrix<T,F,A>    // prod( diagonal m1, m2 )
prod(const diagonal_matrix<T,L,A> &m1, const matrix<T,F,A> &m2 )
{
    #ifdef DEBUG_LINALG 
      std::cout << "\n... ... ... \x1b[0;33mRedirection of [diagonal mat, mat] to [mat, mat]\x1b[0;39m";
    #endif

   const matrix<T,F,A> _m1 = m1;
   return prod(_m1,m2);
}

template<class T, class F, class L, class A>
inline matrix<T,F,A>    // prod( m1, diagonal m2 )
prod(const matrix<T,F,A> &m1, const diagonal_matrix<T,L,A> &m2 )
{
    #ifdef DEBUG_LINALG 
      std::cout << "\n... ... ... \x1b[0;33mRedirection of [mat, diagonal mat] to [mat, mat]\x1b[0;39m";
    #endif

   const matrix<T,F,A> _m2 = m2;
   return prod(m1,_m2);
}

template<class T, class F, class L, class A>
inline matrix<T,F,A>    // prod( diagonal m1, diagonal m2 )
prod(const diagonal_matrix<T,L,A> &m1, const diagonal_matrix<T,L,A> &m2 )
{
    #ifdef DEBUG_LINALG 
      std::cout << "\n... ... ... \x1b[0;33mRedirection of [diagonal mat, diagonal mat] to [mat, mat]\x1b[0;39m";
    #endif

   const matrix<T,F,A> _m1 = m1;
   const matrix<T,F,A> _m2 = m2;
   return prod(_m1,_m2);
}

template<class T, class F, class L, class A>
inline matrix<T,F,A>    // prod( diagonal m1, transpose m2 )
prod(const diagonal_matrix<T,L,A> &m1, const matrix_unary2<matrix<T,F,A>,scalar_identity<T> > &m2)
{
    #ifdef DEBUG_LINALG 
      std::cout << "\n... ... ... \x1b[0;33mRedirection of [diagonal mat, transpose mat] to [mat, mat]\x1b[0;39m";
    #endif

   const matrix<T,F,A> _m1 = m1;
   const matrix<T,F,A> _m2 = m2;

   return prod(_m1,_m2);

}

template<class T, class F, class L, class A>
inline matrix<T,F,A>    // prod( transpose m1, diagonal m2 )
prod(const matrix_unary2<matrix<T,F,A>,scalar_identity<T> > &m1, const diagonal_matrix<T,L,A> &m2)
{
    #ifdef DEBUG_LINALG 
      std::cout << "\n... ... ... \x1b[0;33mRedirection of [transpose mat, diagonal mat] to [mat, mat]\x1b[0;39m";
    #endif

   const matrix<T,F,A> _m1 = m1;
   const matrix<T,F,A> _m2 = m2;

   return prod(_m1,_m2);

}

// symmetric matrix
template<class T, class F, class A, class TRI, class L>
inline matrix<T,F,A>    // prod( symmetric m1, m2 )
prod(symmetric_matrix<T, TRI, L, A> &m1, matrix<T,F,A> &m2 )
{
    #ifdef DEBUG_LINALG 
      std::cout << "\n... ... ... \x1b[0;33mRedirection of [symmetric mat, mat] to [mat, mat]\x1b[0;39m";
    #endif
   assert( m1.size1() == m2.size1() );
   const matrix<T,F,A> _m1 = m1;

   return prod(_m1, m2 );
}

template<class T, class F, class A, class TRI, class L>
inline matrix<T,F,A>    // prod( m1, symmetric m2 )
prod( matrix<T,F,A> &m1, symmetric_matrix<T, TRI, L, A> &m2 )
{
    #ifdef DEBUG_LINALG 
      std::cout << "\n... ... ... \x1b[0;33mRedirection of [mat, symmetric mat] to [mat, mat]\x1b[0;39m";
    #endif
      
   assert( m1.size1() == m2.size1() );
   const matrix<T,F,A> _m2 = m2;

   return prod(m1, _m2 );
}

template<class T, class F, class A, class TRI, class L>
inline matrix<T,F,A>    // prod( symmetric m1, symmetric m2 )
prod(symmetric_matrix<T, TRI, L, A> &m1, symmetric_matrix<T, TRI, L, A> &m2 )
{
    #ifdef DEBUG_LINALG 
      std::cout << "\n... ... ... \x1b[0;33mRedirection of [symmetric mat, symmetric mat] to [mat, mat]\x1b[0;39m";
    #endif

   assert( m1.size1() == m2.size1() );
   const matrix<T,F,A> _m1 = m1;
   const matrix<T,F,A> _m2 = m2;

   return prod(_m1, _m2 );
}




}}}

#endif	/* __VOTCA_CTP_LINALG_H */

