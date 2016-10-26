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
#include <votca/kmc/bnode.h>

namespace votca { namespace kmc {
    
class Electron : public Carrier {
public:
    
    std::string Type(){ return "electron"; } ;
    
    virtual bool Move( Edge* edge ) {
        std::vector<BNode*>::iterator it = NodeOccupation ( edge->NodeTo() ) ;
        
        //  move the electron if the node is free
        if ( it == OccupiedNodes.end() ) {
            distance += edge->DistancePBC();
            SetNode( edge->NodeTo() );
            /// to do
            /// substitute the existing node pointer with the new one
            // to do
            return true;
        // reject the move if it is occupied
        } else {
            return false;
        }
        
        
        
    }
 
private:
    
    /// shared between all nodes information about occupied nodes
    static std::vector<BNode*> OccupiedNodes;
    
    /// returns an iterator to a node [if the node is in the occupied nodes] or the end iterator
    std::vector<BNode*>::iterator NodeOccupation( BNode* node ){  
        return std::find(OccupiedNodes.begin(), OccupiedNodes.end(), node);
    };
    
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        //serialize base class information
        ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Carrier);
        
    }
 
};

}}

BOOST_CLASS_VERSION(votca::kmc::Electron, 0)

#endif