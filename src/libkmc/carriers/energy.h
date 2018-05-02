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

#ifndef __VOTCA_KMC_Energy_H_
#define __VOTCA_KMC_Energy_H_

#include <votca/kmc/carrier.h>
#include <votca/kmc/bnode.h>
#include <votca/kmc/event.h>
#include <votca/tools/globals.h>
#include <iostream>
#include <fstream>

namespace votca { namespace kmc {
    
class Energy : public Carrier {
public:
    
    std::string Type(){ return "energy"; } ;

    virtual bool AddNode( BNode* _node ) { 
        node = _node;
        std::vector<BNode*>::iterator it_node  = NodeOccupation ( node ) ;
        if (it_node == energy_occupiedNodes.end()){    
            //std::cout << " on node: " <<  node->id << std::endl;
            energy_occupiedNodes.push_back( node );
            return true;                
        }
        else {
            //std::cout << " Not added - node already occupied " << std::endl;
            return false; 
        } 
    };  
    
    std::string move_type;
    
    virtual bool Move( Edge* edge ) {
        
        double distance_x = (edge->NodeTo()->position.getX()-edge->NodeFrom()->position.getX())*(edge->NodeTo()->position.getX()-edge->NodeFrom()->position.getX());
        double distance_y = (edge->NodeTo()->position.getY()-edge->NodeFrom()->position.getY())*(edge->NodeTo()->position.getY()-edge->NodeFrom()->position.getY());
        double distance_z = (edge->NodeTo()->position.getZ()-edge->NodeFrom()->position.getZ())*(edge->NodeTo()->position.getZ()-edge->NodeFrom()->position.getZ());
        double hop_distance = sqrt(distance_x+distance_y+distance_z);
        
        if(votca::tools::globals::verbose){
            if (edge->Rate_forster_energy() != 0.00 && edge->Rate_inject() == 0.00 && edge->Rate_collect() == 0.00 && edge->Rate_return() == 0.00)
            {   std::cout << "Förster transfer " 
                << id() << ": " << edge->NodeFrom()->id << "->" << edge->NodeTo()->id
                << " Node_to position: " << edge->NodeTo()->position                     
                << " distance " << hop_distance;
                //<< " Excited (energy) Nodes: " ;
                //for(auto& node : energy_occupiedNodes) { std::cout << node->id << " "; }
                //std::cout << " Total: " << energy_occupiedNodes.size();
            }
            else if (edge->Rate_dexter_energy() != 0.00 && edge->Rate_inject() == 0.00 && edge->Rate_collect() == 0.00 && edge->Rate_return() == 0.00)
            {   std::cout << "Dexter transfer " 
                << id() << ": " << edge->NodeFrom()->id << "->" << edge->NodeTo()->id
                << " Node_to position: " << edge->NodeTo()->position                     
                << " distance " << hop_distance;
                //<< " Excited (energy) Nodes: " ;
                //for(auto& node : energy_occupiedNodes) { std::cout << node->id << " "; }
                //std::cout << " Total: " << energy_occupiedNodes.size();
            }
            else if (edge->Rate_forster_energy() != 0.00 && edge->Rate_collect() != 0.00 )
            { 
                if (edge->Rate_collect() == 2){ std::cout << "Phosphorescence Collect ";}
                else if (edge->Rate_collect() == 1){ std:: cout << "Fluorescence Collect " ;}
                std::cout << id() << ": " << edge->NodeFrom()->id << "->" << edge->NodeTo()->id
                << " Node_to position: " << edge->NodeTo()->position;                     
                //<< " distance " << hop_distance;
                //<< " Excited (energy) Nodes: " ;
                //for(auto& node : energy_occupiedNodes) { std::cout << node->id << " "; }
                //std::cout << " Total: " << energy_occupiedNodes.size();
            }
            else if (edge->Rate_inject() != 0.00 ){std::cout << "Energy INJECT " 
                << id() << ": " << edge->NodeFrom()->id << "->" << edge->NodeTo()->id
                << " Node_to position: " << edge->NodeTo()->position;                   
                //<< " distance " << hop_distance;
                //<< " Excited (energy) Nodes: " ;
                //for(auto& node : energy_occupiedNodes) { std::cout << node->id << " "; }
                //std::cout << " Total: " << energy_occupiedNodes.size();
            }
        }
        
        std::vector<BNode*>::iterator it_to   = NodeOccupation ( edge->NodeTo() ) ;

        if ( it_to == energy_occupiedNodes.end() ) { //  move the energy if the node is free

            if(votca::tools::globals::verbose){
                if (edge->Rate_forster_energy() != 0.00 && edge->Rate_inject() == 0.00 && edge->Rate_collect() == 0.00 && edge->Rate_return() == 0.00)
                { std::cout << " --- Energy Transfered" << std::endl;}
                else if (edge->Rate_dexter_energy() != 0.00 && edge->Rate_inject() == 0.00 && edge->Rate_collect() == 0.00 && edge->Rate_return() == 0.00)
                { std::cout << " --- Energy Transfered" << std::endl;}
                else if (edge->Rate_collect() != 0.00 ){ std:: cout << " --- Energy Collected" << std::endl ;}
                else if (edge->Rate_inject() != 0.00 ) { std::cout << " ---Energy Injected " << std::endl;}

            }
            
            if (edge->Rate_forster_energy() != 0.00 && edge->Rate_inject() == 0.00 && edge->Rate_collect() == 0.00 && edge->Rate_return() == 0.00)
                { move_type="forster";}
            else if (edge->Rate_dexter_energy() != 0.00 && edge->Rate_inject() == 0.00 && edge->Rate_collect() == 0.00 && edge->Rate_return() == 0.00)
                { move_type ="dexter";}
            else if (edge->Rate_collect() != 0.0 )
            { 
                if (edge->Rate_collect() == 2){
                    move_type = "phosphorescence collect" ;
                    
                }
                //std::cout << " collect event" <<std::endl;
                else if (edge->Rate_collect() == 1 ) {
                    move_type = "fluorescence collect" ;
                }
                else {move_type = "collect";}
            }
            
            else if (edge->Rate_inject() != 0.00 ) { move_type = "inject";}
            
            distance += edge->DistancePBC();
            node = edge->NodeTo();
    
            std::vector<BNode*>::iterator it_from = NodeOccupation ( edge->NodeFrom() ) ;
            *it_from =node; 

            return true;
            
        }
        
        else { // reject the move if the node is occupied
            if(votca::tools::globals::verbose){
                if (edge->Rate_return() == 0.00){ std::cout << " --- REJECTED  ";}
            }
            node = edge->NodeFrom();
            if(votca::tools::globals::verbose){
                if (edge->Rate_return() == 0.00){std::cout << "Staying on node: " << node->id << std::endl;}
            }
            return false;
            
        }     
    }
     

    /// shared between all nodes information about occupied (by an energy) nodes
    static std::vector<BNode*> energy_occupiedNodes;
    
    /// returns an iterator to a node [if the node is in the occupied (by an energy) nodes] or the end iterator
    std::vector<BNode*>::iterator NodeOccupation( BNode* node ){  
        return std::find(energy_occupiedNodes.begin(), energy_occupiedNodes.end(), node);
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

BOOST_CLASS_VERSION(votca::kmc::Energy, 0)

#endif