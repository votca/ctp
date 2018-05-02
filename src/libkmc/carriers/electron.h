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
#include <votca/kmc/event.h>
#include <votca/tools/globals.h>

namespace votca { namespace kmc {
    
class Electron : public Carrier {
public:
    
    std::string Type(){ return "electron"; } ;

    //carrier injected to a node if the node is free
    virtual bool AddNode( BNode* _node ) { 
        node = _node;
        std::vector<BNode*>::iterator it_node  = NodeOccupation ( node ) ;
        if (it_node == e_occupiedNodes.end()){    
            std::cout << " on node: " <<  node->id << std::endl;
            e_occupiedNodes.push_back( node );
            return true;                
        }
        else {
            //std::cout << " Not added - node already occupied " << std::endl;
            return false; 
        } 
    };  
    
    //electron transfer move event - if the node to is free (if not the node to is disabled)
    virtual bool Move( Edge* edge ) {
        
        if(votca::tools::globals::verbose){
        std::cout << "Electron " << id() << ": " << edge->NodeFrom()->id << "->" << edge->NodeTo()->id
                      << " Node_to position: " << edge->NodeTo()->position                     
                      //<< " distance "  << edge->NodeTo()->position - edge->NodeFrom()->position
                      << " Occupied (e) Nodes: " ;
                      for(auto& node : e_occupiedNodes) { std::cout << node->id << " "; }
                      std::cout << " Total: " << e_occupiedNodes.size();
        }
        
        std::vector<BNode*>::iterator it_to   = NodeOccupation ( edge->NodeTo() ) ;
        
        if ( it_to == e_occupiedNodes.end() ) { //  move the electron if the node is free

            if(votca::tools::globals::verbose){std::cout << " --- MOVING. " << std::endl;}
            
            //if move allowed, add distance travelled to total distance and move
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
     

    /// shared between all nodes information about occupied (by an electron) nodes
    static std::vector<BNode*> e_occupiedNodes;
    
    /// returns an iterator to a node [if the node is in the occupied (by an electron) nodes] or the end iterator
    std::vector<BNode*>::iterator NodeOccupation( BNode* node ){  
        return std::find(e_occupiedNodes.begin(), e_occupiedNodes.end(), node);
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

BOOST_CLASS_VERSION(votca::kmc::Electron, 0)

#endif