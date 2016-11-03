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

    void AddNode( BNode* _node ) { 
        node = _node; 
        OccupiedNodes.push_back( node );
        std::cout << "Added node " <<  node->id << std::endl;
    };  
    
    virtual bool Move( Edge* edge ) {
        
        std::cout << "Electron " << id() << ": " << edge->NodeFrom()->id << "->" << edge->NodeTo()->id 
                      << " Occupied Nodes: " ;
                      for(auto& node : OccupiedNodes) { std::cout << node->id << " "; }
                      std::cout << " Total: " << OccupiedNodes.size();
        
        
        std::vector<BNode*>::iterator it_to   = NodeOccupation ( edge->NodeTo() ) ;
        
        if ( it_to == OccupiedNodes.end() ) { //  move the electron if the node is free

            std::cout << " --- MOVING. " << std::endl;
            
            distance += edge->DistancePBC();
            node = edge->NodeTo();
            
            // update the list of the occupied (by electrons) nodes
            std::vector<BNode*>::iterator it_from = NodeOccupation ( edge->NodeFrom() ) ;
            *it_from = node;
            
            return true;
            
        } else { // reject the move if the node is occupied
            std::cout << " --- DISABLING. " << std::endl;

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