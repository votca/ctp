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

    std::vector<Event*> to_be_disabled;
    std::vector<Event*> to_be_enabled;

    void AddToToBeDisabled( Event* event ) { 
        std::cout << "Added to the to be disabled list " << node_from->id << " " << node_to->id << std::endl;
        to_be_disabled.push_back( event ); 
    }
    void AddToToBeEnabled( Event* event ) { 
        std::cout << "Added to the to be enabled list " << node_to->id << " " << node_from->id << std::endl;
        to_be_enabled.push_back( event ); 
    }
    
    void DeactivateDisabled() {
       for (std::vector<Event*>::iterator event = to_be_disabled.begin() ; event != to_be_disabled.end(); ++event) {
           (*event)->Disable();
        }
    }
   
    void ActivateEnabled () {
        for (std::vector<Event*>::iterator event = to_be_enabled.begin() ; event != to_be_enabled.end(); ++event) {
            (*event)->Enable();
        }
    }
    
   //virtual double SetRate() = 0;
   
    
    void AddElectron( Carrier* _electron );
    void SetOrigin( BNode* _node );
    void SetDestination( BNode* _node);
   
    // electron transfer rate
    void SetRate( double rate );

    virtual void OnExecute(  State* state ) {
    
        std::cout << "Moving an " << electron->Type() << " " << electron->id() << 
                " from node " <<  node_from->id << 
                " to node "   <<  node_to->id << std::endl;
 
        // disable charge transfer events from the node_from (including itself))
        DeactivateDisabled();
        
        // this is only for a single carrier - rethink for different types
        if ( to_be_enabled.size() == 0 ) {
            for (BNode::iterator node = node_to->begin() ; node != node_to->end(); ++node) {
                //New event - electron transfer
                Event* _et =  Events().Create( "electron_transfer" );
                Electrontransfer* et = new Electrontransfer();
        
                et = dynamic_cast<Electrontransfer*>(_et);
                et->AddElectron( NULL );
                et->SetOrigin( node_to );
                et->SetDestination( *node );
                
                // add to the global even list _et
            }
        }
               
        // enable charge transfer events from the node_to
        ActivateEnabled();
        // move an electron from node_from to node_to
        state->MoveCarrier( electron, node_to );
                
        // in the algorithm the clock should be updated
        
    };
    
private:
    
    std::vector< BNode* > neighbours;
 
};

inline void Electrontransfer::AddElectron( Carrier* _electron )
{
    electron = _electron;
}

inline void Electrontransfer::SetOrigin( BNode* _node )
{
    node_from = _node;   
    std::cout << "from node: " << node_from->id << std::endl;
}

inline void Electrontransfer::SetDestination( BNode* _node )
{
    node_to = _node;   
    std::cout << " to node: " << node_to->id << std::endl;
}

inline void Electrontransfer::SetRate( double _rate )
{
    rate = _rate;
}

}}
#endif 
