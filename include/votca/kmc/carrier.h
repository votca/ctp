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
//#include <votca/kmc/state.h>

namespace votca { namespace kmc {
  
class Carrier {

public: 
    
   Carrier(){};
   virtual ~Carrier(){};     

   virtual std::string Type() = 0;
   
    
private:
    
    int ID;
    // distance travelled
    votca::tools::vec distance;
    // position
    votca::tools::vec position;
    
    
    
    friend class boost::serialization::access;
    
    //serialization itself (template implementation stays in the header)
    std::vector< Carrier* > carriers;
    
    template<typename Archive> 
    void serialize(Archive& ar, const unsigned int version) {

        //version-specific serialization
        if(version == 0)  
        {
            ar & carriers;
            std::cout << "Test" << carriers.size() << std::endl;

        }
    }
      
};

}} 

#endif

