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
#include <votca/kmc/edge.h>
#include "carriers/electron.h"

namespace votca { namespace kmc {
    
class ElectronTransfer : public Event {
    
public:

    // constructor needs two nodes and a pointer to the electron carrier
    // ElectronTransfer( Electron* _electron = NULL, BNode* _node_from = NULL, BNode* _node_to = NULL, double _rate = 0)
    //        : electron(NULL), node_from(NULL), node_to(NULL), rate(0.0)
    //        { electron = _electron; node_from = _node_from; _node_to = node_to; rate = _rate; };
            
    std::string Type(){ return "electron transfer"; } ;
        
    void Initialize( Electron* _electron, Edge* _edge ) {
        // assign electron, nodes, and rate
        electron = _electron;
        edge = _edge;
        node_from = _edge->NodeFrom();
        node_to = _edge->NodeTo();
        distance_pbc = _edge->DistancePBC();
        SetRate( _edge->Rate() );
        // enable this event
        Disable();
        if ( _electron != NULL )  { Enable(); std::cout << "ENABLED" << std::endl; }
    }
    
    // this has to go away eventually
    void SetElectron( Electron* _electron ){ electron = _electron; };
   
    // changes to be made after this event occurs
    virtual void OnExecute(  State* state, votca::tools::Random2 *RandomVariable ) {
    
        // update the parent VSSM group
        Event* parent = GetParent();
        //std::cout << "  parent of " << Type() << " is " << parent->Type() << std::endl;
        
        // Mark all current subordinate events as expired 
        // They will be deleted after OnExecute 
        parent->ExpireSubordinates();
        
        for ( std::vector<ElectronTransfer*>::iterator it = enabled_events.begin(); it != enabled_events.end(); ++it ) {
            parent->AddSubordinate( (*it) );
            (*it)->Initialize( electron, (*it)->edge);
        }
         // move an electron from node_from to node_to
        electron->Move( edge );        
    };
    
private:

    std::vector<ElectronTransfer*> disabled_events;
    std::vector<ElectronTransfer*> enabled_events;

    // electron to move
    Electron* electron;
    // Move from this node
    BNode* node_from;
    // Move to this node
    BNode* node_to;
    Edge* edge;
    votca::tools::vec distance_pbc;

};

}}
#endif 



