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

    BNode* NodeFrom(){ return node_from; };
    BNode* NodeTo(){ return node_to; };
    
    // this has to go away eventually
    void SetElectron( Electron* _electron ){ electron = _electron; };
   
    // changes to be made after this event occurs
    virtual void OnExecute(  State* state, votca::tools::Random2 *RandomVariable ) {
    
        /*
        std::cout << Type() << " of electron " << electron->id() << 
                " from node " << NodeFrom()->id << 
                " to " << NodeTo()->id << std::endl;
        */
        
        // disable old events
        //std::cout << "  Disabling old events" << std::endl;
        for (auto& event: disabled_events ) {
            event->Disable();
            //event->Print("  -- ");
        }
 
        // update the parent VSSM group
        Event* parent = GetParent();
        parent->ClearSubordinate();
        
        // enable new events
        //std::cout << "  Enabling new events" << std::endl;
        for (auto& event: enabled_events ) {
            if ( event == NULL)  { std::cout << "NULL event!" << std::endl; }
            else {
                parent->AddSubordinate( event );
                event->SetElectron(electron);
                event->Enable();
                //event->Print("  ++ ");
                if ( event->GetParent() == NULL ) { std::cout << "No Parent event!" << std::endl; }
            }
        }
        
        // move an electron from node_from to node_to
        //std::cout << "Moving an electron " << electron->id() << std::endl;
        electron->Move( edge );   

    };
    

    // creates a vector of electron transfer events for a specific node and electron
    void CreateEvents( std::vector< ElectronTransfer* >* events, BNode* node, Electron* electron, bool status ) {
            
        for (BNode::EdgeIterator it_edge = node->EdgesBegin() ; it_edge != node->EdgesEnd(); ++it_edge) {
                //New event - electron transfer
                Event* _et =  Events().Create( "electron_transfer" );
                _et->SetParent( GetParent() );
                ElectronTransfer* et = dynamic_cast<ElectronTransfer*>(_et);
                et->Initialize( electron, *it_edge );
                if ( status ) {
                    et->Enable();
                    //std::cout << node->id << "-" << (*node_to)->id << " ";
                } else {
                    et->Disable();
                    //std::cout << node->id << "-" << (*node_to)->id << " ";
                }
                events->push_back(et);
        }
        //std::cout << std::endl;

    }  
    
    void AddEnableOnExecute( std::vector< Event* >* events ) {
        for (auto& event: *events ) {
            ElectronTransfer* ct_transfer = dynamic_cast<ElectronTransfer*>(event);
            enabled_events.push_back(ct_transfer);
        }
    }

    void AddDisableOnExecute( std::vector< Event* >* events ) {
        for (auto& event: *events ) {
            ElectronTransfer* ct_transfer = dynamic_cast<ElectronTransfer*>(event);
            disabled_events.push_back(ct_transfer);
        }
    }

    virtual void Print(std::string offset="") {
        std::cout << offset << Type();
        if ( electron == NULL ) { std:: cout << " no carrier "; } else { std::cout << " carrier id "  << electron->id(); }
        std::cout 
            << " Node "  << node_from->id << "->" << node_to->id  
            << " disabled: " << disabled_events.size() 
            << " enabled: " << enabled_events.size() 
            << " cumulative rate: " << CumulativeRate() <<  std::endl;
    }
    
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



