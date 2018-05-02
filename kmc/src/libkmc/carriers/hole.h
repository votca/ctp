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

#ifndef __VOTCA_KMC_Hole_H_
#define __VOTCA_KMC_Hole_H_

#include <votca/kmc/carrier.h>
#include <votca/kmc/bnode.h>
#include <votca/kmc/event.h>
#include <votca/tools/globals.h>

namespace votca { namespace kmc {
    
class Hole : public Carrier {
public:
    
    std::string Type(){ return "hole"; } ;

    virtual bool AddNode( BNode* _node ) { 
        node = _node;
        std::vector<BNode*>::iterator it_node  = NodeOccupation ( node ) ;
        if (it_node == h_occupiedNodes.end()){    
            std::cout << " on node: " <<  node->id << std::endl;
            h_occupiedNodes.push_back( node );
            return true;                
        }
        else {
            //std::cout << " Not added - node already occupied " << std::endl;
            return false; 
        } 
    };  
       
    virtual bool Move( Edge* edge ) {
        
        if(votca::tools::globals::verbose){
        std::cout << "Hole " << id() << ": " << edge->NodeFrom()->id << "->" << edge->NodeTo()->id
                      << " Node_to position: " << edge->NodeTo()->position
                      << " Occupied (h) Nodes: " ;
                      for(auto& node : h_occupiedNodes) { std::cout << node->id << " "; }
                      std::cout << " Total: " << h_occupiedNodes.size();
        }
        
        std::vector<BNode*>::iterator it_to   = NodeOccupation ( edge->NodeTo() ) ;
        
        if ( it_to == h_occupiedNodes.end() ) { //  move the hole if the node is free

            if(votca::tools::globals::verbose){std::cout << " --- MOVING. " << std::endl;}
            
            distance += edge->DistancePBC();
            node = edge->NodeTo();
    
            std::vector<BNode*>::iterator it_from = NodeOccupation ( edge->NodeFrom() ) ;
            *it_from =node; 

            return true;
            
        }

        
        else { // reject the move if the node is occupied
            if(votca::tools::globals::verbose){std::cout << " --- REJECTED  ";}
            node = edge->NodeFrom();
            if(votca::tools::globals::verbose){std::cout << "Staying on node: " << node->id << std::endl;}
            
            return false;
        }   
       
    }
     

    /// shared between all nodes information about occupied (by an hole) nodes
    static std::vector<BNode*> h_occupiedNodes;
    
    /// returns an iterator to a node [if the node is in the occupied (by an hole) nodes] or the end iterator
    std::vector<BNode*>::iterator NodeOccupation( BNode* node ){  
        return std::find(h_occupiedNodes.begin(), h_occupiedNodes.end(), node);
    };
    
private:   
    
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        //serialize base class information
        ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Carrier);
        
    }
   
};
    
}}

BOOST_CLASS_VERSION(votca::kmc::Hole, 0)

#endif