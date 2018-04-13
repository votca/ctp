/* 
 *            Copyright 2009-2012 The VOTCA Development Team
 *                       (http://www.votca.org)
 *
 *      Licensed under the Apache License, Version 2.0 (the "License")
 *
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef __VOTCA_CTP_ORBITALS_H
#define	__VOTCA_CTP_ORBITALS_H
// #include <votca/ctp/basisset.h>
// #include <votca/ctp/aobasis.h>
#include <votca/ctp/qmatom.h>

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/symmetric.hpp>
#include <boost/numeric/ublas/io.hpp>

#include <votca/tools/globals.h>
#include <votca/tools/property.h>
#include <votca/tools/vec.h>

#include <votca/ctp/logger.h>

// Text archive that defines boost::archive::text_oarchive
// and boost::archive::text_iarchive
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

// XML archive that defines boost::archive::xml_oarchive
// and boost::archive::xml_iarchive
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>

// XML archive which uses wide characters (use for UTF-8 output ),
// defines boost::archive::xml_woarchive
// and boost::archive::xml_wiarchive
#include <boost/archive/xml_woarchive.hpp>
#include <boost/archive/xml_wiarchive.hpp>

// Binary archive that defines boost::archive::binary_oarchive
// and boost::archive::binary_iarchive
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include <boost/serialization/version.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>

namespace ub = boost::numeric::ublas;
    
namespace votca { namespace ctp {
    
 
/**
 * \brief container for molecular orbitals
 * 
 * The Orbitals class stores orbital id, energy, MO coefficients, basis set
 *     
 */
class Orbitals 
{
public:   

    Orbitals();
   ~Orbitals();

   /*
    * 
    *  ************** NEW ACCESS STRATEGY ****************
    * 
    *  Scalars:              get and set functions
    *  Vectors and matrixes: const and non-const refs, has-function via size
    */
   
    // access to DFT basis set size, new, tested
    bool           hasBasisSetSize() { return ( _basis_set_size > 0 ) ? true : false ; }
    int            getBasisSetSize() { return _basis_set_size ;}
    void           setBasisSetSize( const int &basis_set_size ) {_basis_set_size = basis_set_size;}

    // access to DFT number of levels, new, tested
    bool           hasNumberOfLevels() { return ( (_occupied_levels > 0) && (_unoccupied_levels >0 ) ? true : false );}
    int            getNumberOfLevels() { return ( _occupied_levels + _unoccupied_levels ) ; }
    void           setNumberOfLevels( const int &occupied_levels, const int &unoccupied_levels );
    
    // access to DFT number of electrons, new, tested
    bool           hasNumberOfElectrons() { return ( _number_of_electrons > 0 ) ? true : false ; }
    int            getNumberOfElectrons() { return  _number_of_electrons ; } ;
    void           setNumberOfElectrons( const int &electrons ) { _number_of_electrons = electrons;}

    // access to QM package name, new, tested
    bool hasQMpackage() { return (!_qm_package.empty()); }
    std::string getQMpackage() { return _qm_package; }
    void setQMpakckage( std::string qmpackage ) { _qm_package = qmpackage;}

    // access to DFT AO overlap matrix, new, tested
    bool           hasAOOverlap() { return ( _overlap.size1() > 0 ) ? true : false ;}
    const ub::symmetric_matrix<double> &AOOverlap() const { return _overlap; }
    ub::symmetric_matrix<double> &AOOverlap() { return _overlap; }
    ub::symmetric_matrix<double>* getOverlap() { return &_overlap; } // OLD

    // access to DFT molecular orbital energies, new, tested
    bool          hasMOEnergies() { return ( _mo_energies.size() > 0 ) ? true : false ;}
    const ub::vector<double> &MOEnergies() const { return _mo_energies; }
    ub::vector<double> &MOEnergies() { return _mo_energies; }
    ub::vector<double>* getEnergies() { return &_mo_energies; } // OLD

    // access to DFT molecular orbital energy of a specific level (in eV)
    double getEnergy( int level) { return ( hasMOEnergies() ) ? tools::globals::conversion::Hrt2eV*_mo_energies[level-1] : 0; }

    // access to DFT molecular orbital coefficients, new, tested
    bool          hasMOCoefficients() { return ( _mo_coefficients.size1() > 0 ) ? true : false ;}
    const ub::matrix<double> &MOCoefficients() const { return _mo_coefficients; }
    ub::matrix<double> &MOCoefficients() { return _mo_coefficients; }
    ub::matrix<double>* getOrbitals() { return &_mo_coefficients; } //OLD

    // access to DFT transfer integrals, new, tested
    bool hasMOCouplings() { return ( _mo_couplings.size1() > 0 ) ? true : false ;}
    const ub::matrix<double> &MOCouplings() const { return _mo_couplings; }
    ub::matrix<double> &MOCouplings() { return _mo_couplings; }
    
   
    // determine (pseudo-)degeneracy of a DFT molecular orbital
    bool hasDegeneracy() { return ( !_level_degeneracy.empty() ) ? true : false;}
    std::vector<int>* getDegeneracy( int level, double _energy_difference );

    // access to QM atoms
    //bool hasQMAtoms() { return _has_atoms;}
    bool hasQMAtoms() { return ( _atoms.size() > 0 ) ? true : false ;}
    // void setQMAtoms( bool inp ) { _has_atoms = inp;}
    const std::vector< QMAtom* > &QMAtoms() const { return _atoms ;}
    std::vector< QMAtom* > &QMAtoms()  { return _atoms ;}
    std::vector< QMAtom* >* getAtoms() { return &_atoms; } //OLD
    
    // access to classical self-energy in MM environment, new, tested
    bool hasSelfEnergy() { return ( _self_energy != 0.0 ) ? true : false ; }
    double getSelfEnergy() { return  _self_energy ; }
    void setSelfEnergy( double selfenergy ) { _self_energy = selfenergy;}

    // access to QM total energy, new, tested
    bool hasQMEnergy() { return ( _qm_energy != 0.0 ) ? true : false ; }
    double getQMEnergy() { return  _qm_energy ; }
    void setQMEnergy( double qmenergy) { _qm_energy = qmenergy;}
  
    


    
    // returns indeces of a re-sorted in a descending order vector of energies
    void SortEnergies( std::vector<int>* index );
    
    /** Adds a QM atom to the atom list */
    QMAtom* AddAtom (std::string _type, 
                     double _x, double _y, double _z, 
                     double _charge = 0, bool _from_environment = false)
    {
        QMAtom* pAtom = new QMAtom(_type, _x, _y, _z, _charge, _from_environment);
        _atoms.push_back( pAtom );
        return pAtom;
    }
    
    void setStorage( bool _store_orbitals, bool _store_overlap,  bool _store_integrals ) {
        // _has_mo_coefficients = _store_orbitals;
        //hasOverlap() = _store_overlap;
        // _has_integrals = _store_integrals;
        ;
    } 
        
    void WritePDB( FILE *out );
    
    // reduces number of virtual orbitals to factor*number_of_occupied_orbitals
    void Trim( int factor );

    // reduces number of virtual orbitals to [HOMO-degG:LUMO+degL]
    void Trim( int degH, int degL );

    /** Loads orbitals from a file
     * Returns true if successful and does not throw an exception.
     * If exception is required, please use the << overload.
     */
    bool Load(std::string file_name);    
    
private:
    
    

    int                                     _basis_set_size;   
    int                                     _occupied_levels;
    int                                     _unoccupied_levels;
    int                                     _number_of_electrons;
    
    std::map<int, std::vector<int> >        _level_degeneracy;
    
    ub::vector<double>                      _mo_energies; 
    ub::matrix<double>                      _mo_coefficients;

    ub::symmetric_matrix<double>            _overlap;
    //ub::symmetric_matrix<double>            _vxc;
    
    std::vector< QMAtom* >                  _atoms;   

    double                                  _qm_energy;
    double                                  _self_energy;
    
    ub::matrix<double>                      _mo_couplings;
   
    bool                                    _has_basis_set;
     
    std::string                                  _qm_package;
   
    
private:

    /**
    * @param _energy_difference [ev] Two levels are degenerate if their energy is smaller than this value
    * @return A map with key as a level and a vector which is a list of close lying orbitals
    */    
    bool CheckDegeneracy( double _energy_difference );
    
    // Allow serialization to access non-public data members
    friend class boost::serialization::access;
    
    //Allow Gaussian object to access non-public data members
    friend class Gaussian;
    friend class Turbomole;
    friend class NWChem;
    friend class GW;
    
    // serialization itself (template implementation stays in the header)
    template<typename Archive> 
    void serialize(Archive& ar, const unsigned int version) {

       ar & _basis_set_size;
       ar & _occupied_levels; 
       ar & _unoccupied_levels;
       ar & _number_of_electrons; 
       ar & _level_degeneracy;
       ar & _mo_energies;
       ar & _mo_coefficients;

       
       // symmetric matrix does not serialize by default
       if (Archive::is_saving::value) {
           unsigned int size = _overlap.size1();
           ar & size;
       }

      // copy the values back if loading
      if (Archive::is_loading::value) {
          unsigned int size;
          ar & size;
          _overlap.resize(size);
      }
            
     for (unsigned int i = 0; i < _overlap.size1(); ++i)
         for (unsigned int j = 0; j <= i; ++j)
             ar & _overlap(i, j); 

     ar & _atoms; 
     ar & _qm_energy; 
     ar & _qm_package;
     ar & _self_energy; 
     ar & _mo_couplings;

  
    }// end of serialization
};

}}

BOOST_CLASS_VERSION(votca::ctp::Orbitals, 1)
        
#endif	/* __VOTCA_CTP_ORBITALS_H */

