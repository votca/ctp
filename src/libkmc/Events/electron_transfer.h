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
    
class ElectronTransfer : public Event {
    
public:

    // constructor needs two nodes and a pointer to the electron carrier
    ElectronTransfer( Electron* _electron = NULL, BNode* _node_from = NULL, BNode* _node_to = NULL, double _rate = 0)
            : electron(NULL), node_from(NULL), node_to(NULL), rate(0.0)
            { electron = _electron; node_from = _node_from; _node_to = node_to; rate = _rate; };
            
    std::string Type(){ return "electron transfer"; } ;
    
    // electron to move
    Carrier* electron;
    // Move from this node
    BNode* node_from;
    // Move to this node
    BNode* node_to;
    //rate of electron transfer
    double rate;
    
    void Initialize( Electron* _electron, BNode* _node_from, BNode* _node_to, double _rate ) {
        // create a list of events to disable after OnExecute
        disabled_events = CreateEvents( _node_from, _electron, false );
        // create a list of events to enable after OnExecute
        enabled_events = CreateEvents( _node_to, _electron, true );
        // assign electron, nodes, and rate
        electron = _electron;
        node_from = _node_from;
        node_to = _node_to;
        rate = _rate;
        // enable this event
        Enable();
    }
    
    // this has to go away eventually
    void AddElectron( Carrier* _electron );
    void SetOrigin( BNode* _node );
    void SetDestination( BNode* _node);
   
    // electron transfer rate
    void SetRate( double _rate ) { rate = _rate; };
    double Rate(){ return rate; }

    // changes to be made after this event occurs
    virtual void OnExecute(  State* state ) {
    
        std::cout << "Moving " << electron->Type() << " " << electron->id() << 
                " from node " <<  node_from->id << 
                " to node "   <<  node_to->id << std::endl;
 
        // disable charge transfer events from the node_from (including itself))
        DisableEvents();  
                
        // if there are no to_be_enabled events, create them
        if ( enabled_events.size() == 0 ) {            
            enabled_events = CreateEvents( node_to, NULL, true );
        } else {  // enable charge transfer events from the node_to
            EnableEvents();
        }
        
        // move an electron from node_from to node_to
        state->MoveCarrier( electron, node_to );
                
        // in the algorithm the clock should be updated
        
    };
    
private:

    std::vector<ElectronTransfer*> disabled_events;
    std::vector<ElectronTransfer*> enabled_events;


    // disable events after OnExecute
    void DisableEvents() {
        std::cout << "Deactivating " << disabled_events.size() << " events" << std::endl;
        
        for (std::vector<ElectronTransfer*>::iterator event = disabled_events.begin() ; event != disabled_events.end(); ++event) {
           (*event)->Disable();
           std::cout << (*event)->node_from->id << "-" << (*event)->node_to->id << " ";
        }
        std::cout << endl;
    }
   
    // enable events OnExecute
    void EnableEvents() {
        std::cout << "Activating " << enabled_events.size() << " events" << std::endl;
        for (std::vector<ElectronTransfer*>::iterator event = enabled_events.begin() ; event != enabled_events.end(); ++event) {
            (*event)->Enable();
            std::cout << (*event)->node_from->id << "-" << (*event)->node_to->id << " ";
        }
        std::cout << endl;
    }
    
    // creates a vector of electron transfer events for a specific node and electron
    std::vector< ElectronTransfer* > CreateEvents( BNode* node, Electron* electron, bool status ) {
        
        std::vector<ElectronTransfer*> events;

        std::cout << "Creating events for charge " << electron->id() << ", node " << node->id << std::endl;
        //node->PrintNode();
        
        if (status) {
            std::cout << "To be enabled: " ;
        } else {
            std::cout << "To be disabled: ";
        }
            
        for (BNode::iterator node_to = node->begin() ; node_to != node->end(); ++node_to) {
                //New event - electron transfer
                Event* _et =  Events().Create( "electron_transfer" );
                ElectronTransfer* et = dynamic_cast<ElectronTransfer*>(_et);
                et->SetOrigin( node );
                et->AddElectron( electron );
                et->SetDestination( *node_to );
                if ( status ) {
                    et->Enable();
                    std::cout << node->id << "-" << (*node_to)->id << " ";
                } else {
                    et->Disable();
                    std::cout << node->id << "-" << (*node_to)->id << " ";
                }
                events.push_back(et);
        }
        std::cout << std::endl;
        return events;
    }
    
    
    std::vector< BNode* > neighbours;
 
};

inline void ElectronTransfer::AddElectron( Carrier* _electron )
{
    electron = _electron;
}

inline void ElectronTransfer::SetOrigin( BNode* _node )
{
    node_from = _node;   
    //std::cout << "from node: " << node_from->id << std::endl;
}

inline void ElectronTransfer::SetDestination( BNode* _node )
{
    node_to = _node;   
    //std::cout << " to node: " << node_to->id << std::endl;
}


}}
#endif 
