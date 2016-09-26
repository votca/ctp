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

#ifndef __VOTCA_KMC_CARRIER_H_
#define __VOTCA_KMC_CARRIER_H_

#include <votca/tools/vec.h>
#include "bnode.h"

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

namespace votca { namespace kmc {
  
class Carrier {

public: 
    
   Carrier(){};
   virtual ~Carrier(){};     

   virtual std::string Type() = 0;
   
   virtual int id(){ return ID; };
    
private:
    
    int ID;
    // distance travelled
    votca::tools::vec distance;
    // position
    votca::tools::vec position;
    // node on which it resides
    BNode* node;
 
/*    
    friend class boost::serialization::access;
   
    
    template<typename Archive> 
    void serialize(Archive& ar, const unsigned int version) {
        
        //version-specific serialization
        if(version == 0)  
        {
            ar & id;
            ar & node->id;

        }
    }
*/
    
};

}} 

BOOST_CLASS_VERSION(votca::kmc::Carrier, 0)
BOOST_SERIALIZATION_ASSUME_ABSTRACT( Carrier );

#endif

