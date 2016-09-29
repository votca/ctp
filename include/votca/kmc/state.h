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
    
    // Save and Load state into a file
    bool Save(std::string filename);
    bool Load(std::string filename);
    
    Carrier* AddCarrier( std::string type );
    
    void MoveCarrier( Carrier* carrier, BNode* node );
    
private:
    // Allow serialization to access non-public data members
    friend class boost::serialization::access;
    
    std::vector< Carrier* > carriers;

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
    carriers.push_back( carrier );
    return carrier;
}

inline void State::MoveCarrier( Carrier* carrier, BNode* node_to ) {
    carrier->SetNode( node_to );
    // and advance its position travelled
}

}} 

BOOST_CLASS_VERSION(votca::kmc::State, 0)

#endif

