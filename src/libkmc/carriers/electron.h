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

#ifndef __VOTCA_KMC_Electron_H_
#define __VOTCA_KMC_Electron_H_

#include <votca/kmc/carrier.h>

namespace votca { namespace kmc {
    
class Electron : public Carrier {
public:
    
    std::string Type(){ return "electron"; } ;
    
    //Should electron have its own ID, aside from the carrier
    int id(){ return ID; };

private:
    
    int ID;
    votca::tools::vec distance;
    votca::tools::vec position;
    BNode* node;
  
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        // serialize base class information
        //ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Carrier);
    }
 
};

}}

BOOST_CLASS_VERSION(votca::kmc::Electron, 0)


#endif