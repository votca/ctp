/*
 * Copyright 2009-2013 The VOTCA Development Team (http://www.votca.org)
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

#ifndef __VOTCA_KMC_STATE_H_
#define __VOTCA_KMC_STATE_H_

#include <fstream>

//#include <votca/kmc/event.h>
//#include <votca/kmc/eventfactory.h>

// Text archive that defines boost::archive::text_oarchive
// and boost::archive::text_iarchive
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

// XML archive that defines boost::archive::xml_oarchive
// and boost::archive::xml_iarchive
//#include <boost/archive/xml_oarchive.hpp>
//#include <boost/archive/xml_iarchive.hpp>

// XML archive which uses wide characters (use for UTF-8 output ),
// defines boost::archive::xml_woarchive
// and boost::archive::xml_wiarchive
//#include <boost/archive/xml_woarchive.hpp>
//#include <boost/archive/xml_wiarchive.hpp>

// Binary archive that defines boost::archive::binary_oarchive
// and boost::archive::binary_iarchive
//#include <boost/archive/binary_oarchive.hpp>
//#include <boost/archive/binary_iarchive.hpp>

#include <boost/serialization/version.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>

#include <votca/kmc/carrier.h>
#include <votca/kmc/carrierfactory.h>

namespace votca { namespace kmc {
  
class State {
public:
    
    State(){ time = 0; }
    
    // iterator over carriers
    typedef std::vector< Carrier* >::iterator iterator;
    typedef const std::vector< Carrier* >::iterator const_iterator;
    
    iterator begin() { return carriers.begin(); }
    iterator end() { return carriers.end(); }    
    
    // Save and Load state into a file
    bool Save(std::string filename);
    bool Load(std::string filename);
    
    Carrier* AddCarrier( std::string type );
    
    void Print();
    
    void AdvanceClock( double elapsed_time ) { time += elapsed_time; };
    
    Carrier* Node_Occupation( BNode* node ) {
        for ( State::iterator it_carrier = carriers.begin(); it_carrier != carriers.end(); ++it_carrier ) {
            Carrier* carrier = *it_carrier;
            if ( carrier->GetNode() == node ) { 
                return carrier;
            }
        } 
        return NULL;
    }
    
private:
    // Allow serialization to access non-public data members
    friend class boost::serialization::access;
    
    std::vector< Carrier* > carriers;
    
    double time;

     // serialization itself (template implementation stays in the header)
    template<typename Archive> 
    void serialize(Archive& ar, const unsigned int version) {

        // version-specific serialization
        if(version == 0)  {
            ar & carriers;
            std::cout << "Serialized " <<  carriers.size() << " carriers"<< std::endl;
        } 
    }
    
};

inline bool State::Save(std::string filename){
    
    try {
        std::ofstream ofs( filename.c_str() );
        boost::archive::text_oarchive oa( ofs );
        oa << *this;
        ofs.close();
    } catch(std::exception &err) {
        std::cerr << "Could not write state from " << filename << flush; 
        std::cerr << " An error occurred:\n" << err.what() << endl;
        return false;
    } 
    return true;    
    
}

inline bool State::Load(std::string filename){
    
    try {
        std::ifstream ifs( filename.c_str() );
        boost::archive::text_iarchive ia( ifs );
        ia >> *this;
        ifs.close();
    } catch(std::exception &err) {
        std::cerr << "Could not load state from " << filename << flush; 
        std::cerr << "An error occurred:\n" << err.what() << endl;
        return false;
    } 
    return true;
}

// Add a carrier of a certain type
inline Carrier* State::AddCarrier( std::string type ) {

    cout << "Adding carrier " << type << endl;
    Carrier *carrier = Carriers().Create( type );
    carrier->SetID(carriers.size()+1);
    carriers.push_back( carrier );
    return carrier;
}


inline void State::Print(){
    //std::cout << "State has " << carriers.size() << " carriers"<< std::endl;
    std::cout << "Time: " << time << std::endl;
    Carrier* carrier;
    for ( State::iterator it_carrier = carriers.begin(); it_carrier != carriers.end(); ++it_carrier ) {
        carrier = *it_carrier;
        std::cout << "Carrier " << carrier->id() << " of type " << carrier->Type() 
                  << " at node " << carrier->GetNode()->id << " Distance " 
                  << carrier->Distance() << std::endl;
    } 
}
}} 

BOOST_CLASS_VERSION(votca::kmc::State, 0)

#endif

