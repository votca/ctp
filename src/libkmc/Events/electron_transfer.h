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

#ifndef __VOTCA_KMC_ELECTRONTRANSFER_H
#define __VOTCA_KMC_ELECTRONTRANSFER_H

#include <votca/kmc/event.h>
#include <votca/kmc/carrier.h>
#include "carriers/electron.h"

namespace votca { namespace kmc {
    
class Electrontransfer : public Event {
    
public:
    
    std::string Type(){ return "electron transfer"; } ;
    
    // electron to move
    Carrier* electron;
    // Move from this node
    BNode* node_from;
    // Move to this node
    BNode* node_to;
    //rate of electron transfer
    double rate;
    
    void AddCarrier( Carrier* electron );
    //Also a RemoveCarrier?

    void AddOrigin( Electron* node );
   
    //AddDestination( electron->Node->Neighbour() );
    void AddDestination( Electron* neighbour_node);
   
    // electron transfer rate
    double SetRate();

    virtual void OnExecute(  State* state ) {
    
        std::cout << "Moving an " << electron->Type() << " " << electron->id() << 
                " from node " <<  node_from->id << 
                " to node "   <<  node_to->id << std::endl;
    };
    
private:
    
    std::vector< BNode* > neighbours;
 
};

void AddCarrier( Carrier* electron )
{
    //Carrier->Type() = electron;
    //Carrier->id() = electron->id();
    
    State state;
    state.AddCarrier("electron");
    
}

void Electrontransfer::AddOrigin( Electron* node )
{
    //assign node electron->node();
    std::cout << "Electron sits on node: " << electron->id();
    //electron->id() = node_from->id;   
}

void Electrontransfer::AddDestination( Electron* neighbour_node )
{
    //Loop over all neighbouring nodes
    node_from->PrintNode(); 
    for (std::vector< BNode* >::iterator node = neighbours.begin() ; node != neighbours.end(); ++node) 
    {  
    
    }     
    
    //Choose a neighbouring node to move to and get id
    neighbour_node->id();
    
    //Make the move
    
    //If node already occupied, node should be added to a forbidden list
    
    //neighbour_node->id() = node_to->id;
    std::cout << "Electron moved to node: " << node_to->id << std::endl;
    
    //Have to update electron->node - node_to becomes node_from
    //go to the neighbouring nodes for the new node
    
    
    //Update tree - what has to be updated after the carrier moves
    
}

double Electrontransfer::SetRate()
{
    //From the graph - SQL file PAIRS table
    
    //Rate of electron(e) transfer from seg 1 to seg 2
    //double rate12e = stmt->Column<int>(2);
    //Rate of electron transfer from seg 2 to seg 1
    //double rate21e = stmt->Column<int>(3);
    
     
    
}

}}
#endif 
