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

namespace votca { namespace ctp {
    
inline void linalg_invert( const ub::matrix<double> &A, ub::matrix<double> &V){
    throw "Inversion of a matrix, linalg_invert, is not implemented in eigen.h";
}


inline void linalg_eigenvalues(const ub::matrix<double> &A, ub::vector<double> &E, ub::matrix<double> &V)
{
    throw "Eigenvalues of a matrix, linalg_eigenvalues, is not implemented in eigen.h";        
}

}}

namespace boost { namespace numeric { namespace ublas {
    
    // partial specialization for double precision
    template<class F, class A>
    inline matrix<double,F,A>    // prod( m1, m2 )
    prod(const matrix<double,F,A> &m1, const matrix<double,F,A> &m2)
    {
       //#ifdef GSLDEBUG 
          std::cout << "\x1b[0;34mEIGEN PRODUCT [CMD,CMD]\x1b[0;39m\n" << std::flush;
       //#endif
          
       boost::numeric::ublas::matrix<double,F,A> AxB( m1.size1(), m2.size2() );
       
       // Note that a Map<Type> m(&data) gives a view into the data (can change it)
       // while assignments Matrix<Type> m = Map< Matrix<Type> > (&data)
       // lead to a data copy and lost connection between the reference and matrix
       Eigen::Map<const Eigen::MatrixXd> mA_( &(m1.data()[0]), m1.size2(), m1.size1() );
       Eigen::Map<const Eigen::MatrixXd> mB_( &(m2.data()[0]), m2.size2(), m2.size1() );
       Eigen::Map<Eigen::MatrixXd> AxB_( &(AxB.data()[0]), AxB.size2(), AxB.size1() );

       AxB_ = mB_*mA_;
      
       return AxB;
       
    }    
    
    // partial specialization for single precision
    template<class F, class A>
    inline matrix<float,F,A>    // prod( m1, m2 )
    prod(const matrix<float,F,A> &m1, const matrix<float,F,A> &m2)
    {
       //#ifdef GSLDEBUG 
          std::cout << "\x1b[0;34mEIGEN [CMF,CMF]\x1b[0;39m\n" << std::flush;
       //#endif
       boost::numeric::ublas::matrix<float,F,A> AxB( m1.size1(), m2.size2() );

       Eigen::Map<const Eigen::MatrixXf> mA_( &(m1.data()[0]), m1.size2(), m1.size1() );
       Eigen::Map<const Eigen::MatrixXf> mB_( &(m2.data()[0]), m2.size2(), m2.size1() );
       Eigen::Map<Eigen::MatrixXf> AxB_( &(AxB.data()[0]), AxB.size2(), AxB.size1() );

       AxB_ = mB_*mA_;

       return AxB;
    }     
       
    /// transpose products
    template<class T, class F, class A>
    inline matrix<T,F,A>    // prod( trans(m1), m2 )
    prod(const matrix_unary2<matrix<T,F,A>,scalar_identity<T> > &m1, const matrix<T,F,A> &m2)
    {
       //#ifdef GSLDEBUG 
          std::cout << "\x1b[0;32mGSL [CMU,CM]\x1b[0;39m -> " << std::flush;
       //#endif
       boost::numeric::ublas::matrix<T,F,A> _m1 = m1;
       return prod(_m1,m2);
    }    
    
    template<class T, class F, class A>
    inline matrix<T,F,A>    // prod( m1, trans(m2) )
    prod(const matrix<T,F,A> &m1, const matrix_unary2<matrix<T,F,A>,scalar_identity<T> > &m2)
    {
       //#ifdef GSLDEBUG 
          std::cout << "\x1b[0;32mGSL [CM,CTM]\x1b[0;39m -> " << std::flush;
       //#endif
          
       const boost::numeric::ublas::matrix<T,F,A> _m2 = m2;
       return prod(m1,_m2);
    }  
    
    template<class T, class F, class A>
    inline matrix<T,F,A>    // prod( trans(m1), trans(m2) )
    prod(const matrix_unary2<matrix<T,F,A>,scalar_identity<T> > &m1, const matrix_unary2<matrix<T,F,A>,scalar_identity<T> > &m2)    
    {
       //#ifdef GSLDEBUG 
          std::cout << "\x1b[0;32mGSL [CTM,CTM]\x1b[0;39m -> " << std::flush;
       //#endif
          
       boost::numeric::ublas::matrix<T,F,A> _m1 = m1;
       boost::numeric::ublas::matrix<T,F,A> _m2 = m2;
       
       return prod(_m1,_m2);
    }  

    /// diagonal matrix
    template<class T, class F, class L, class A>
    inline matrix<T,F,A>    // prod( diagonal m1, m2 )
    prod(const diagonal_matrix<T,L,A> &m1, const matrix<T,F,A> &m2 )       
    {
       //#ifdef GSLDEBUG 
          std::cout << "\x1b[0;33mGSL [CDM, CM]\x1b[0;39m -> " << std::flush;
       //#endif
       const matrix<T,F,A> _m1 = m1; 
       return prod(_m1,m2);
    }

    template<class T, class F, class L, class A>
    inline matrix<T,F,A>    // prod( m1, diagonal m2 )
    prod(const matrix<T,F,A> &m1, const diagonal_matrix<T,L,A> &m2 )       
    {
       //#ifdef GSLDEBUG 
          std::cout << "\x1b[0;33mGSL [CM, CDM]\x1b[0;39m -> " << std::flush;
       //#endif
       const matrix<T,F,A> _m2 = m2; 
       return prod(m1,_m2);
    }

    template<class T, class F, class L, class A>
    inline matrix<T,F,A>    // prod( diagonal m1, diagonal m2 )
    prod(const diagonal_matrix<T,L,A> &m1, const diagonal_matrix<T,L,A> &m2 )       
    {
       //#ifdef GSLDEBUG 
          std::cout << "\x1b[0;33mGSL [CDM, CM]\x1b[0;39m -> " << std::flush;
       //#endif
       const matrix<T,F,A> _m1 = m1; 
       const matrix<T,F,A> _m2 = m2; 
       return prod(_m1,_m2);
    }    
    
    template<class T, class F, class L, class A>
    inline matrix<T,F,A>    // prod( diagonal m1, transpose m2 )
    prod(const diagonal_matrix<T,L,A> &m1, const matrix_unary2<matrix<T,F,A>,scalar_identity<T> > &m2)   
    {
       //#ifdef GSLDEBUG 
          std::cout << "\x1b[0;33mGSL [CDM, CTM]\x1b[0;39m -> " << std::flush;
       //#endif

       const matrix<T,F,A> _m1 = m1; 
       const matrix<T,F,A> _m2 = m2; 
       
       return prod(_m1,_m2);
       
    }

   template<class T, class F, class L, class A>
    inline matrix<T,F,A>    // prod( transpose m1, diagonal m2 )
    prod(const matrix_unary2<matrix<T,F,A>,scalar_identity<T> > &m1, const diagonal_matrix<T,L,A> &m2)   
    {
       //#ifdef GSLDEBUG 
          std::cout << "\x1b[0;33mGSL [CTM, CDM]\x1b[0;39m -> " << std::flush;
       //#endif

       const matrix<T,F,A> _m1 = m1; 
       const matrix<T,F,A> _m2 = m2; 
       
       return prod(_m1,_m2);
       
    }
    
    // symmetric matrix 
    template<class T, class F, class A, class TRI, class L>
    inline matrix<T,F,A>    // prod( symmetric m1, m2 )
    prod(symmetric_matrix<T, TRI, L, A> &m1, matrix<T,F,A> &m2 )   
    {
       //#ifdef GSLDEBUG 
          std::cout << "\n... ... ... \x1b[0;33mGSL PRODUCT [CSM, CM]\x1b[0;39m -> " << std::flush;
       //#endif
          
       assert( m1.size1() == m2.size1() );
       const matrix<T,F,A> _m1 = m1; 
       
       return prod(_m1, m2 );
    }   

    template<class T, class F, class A, class TRI, class L>
    inline matrix<T,F,A>    // prod( m1, symmetric m2 )
    prod( matrix<T,F,A> &m1, symmetric_matrix<T, TRI, L, A> &m2 )   
    {
       //#ifdef GSLDEBUG 
          std::cout << "\x1b[0;33mGSL [CM, CSM]\x1b[0;39m -> " << std::flush;
       //#endif
          
       assert( m1.size1() == m2.size1() );
       const matrix<T,F,A> _m2 = m2; 
       
       return prod(m1, _m2 );
    } 

    template<class T, class F, class A, class TRI, class L>
    inline matrix<T,F,A>    // prod( symmetric m1, symmetric m2 )
    prod(symmetric_matrix<T, TRI, L, A> &m1, symmetric_matrix<T, TRI, L, A> &m2 )   
    {
       //#ifdef GSLDEBUG 
          std::cout << "\x1b[0;33mGSL [CSM, CSM]\x1b[0;39m -> " << std::flush;
       //#endif
          
       assert( m1.size1() == m2.size1() );
       const matrix<T,F,A> _m1 = m1; 
       const matrix<T,F,A> _m2 = m2; 
       
       return prod(_m1, _m2 );
    } 
        
}}}

#endif  // __VOTCA_CTP_EIGEN_H   
