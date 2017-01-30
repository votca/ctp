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

#ifndef __VOTCA_KMC_ELECTRONTRANSFERSNAIL_H
#define __VOTCA_KMC_ELECTRONTRANSFERSNAIL_H

#include <votca/kmc/event.h>
#include <votca/kmc/edge.h>
#include "carriers/electron.h"

namespace votca { namespace kmc {
    
class ElectronTransferSnail : public Event {
    
public:

    std::string Type(){ return "electron transfer snail"; } ;
        
    void Initialize( Electron* _electron, Edge* _edge ) {
        electron = _electron;
        edge = _edge;
        distance_pbc = _edge->DistancePBC();
        SetRate( _edge->Rate() );
        // enable this event if a carrier is provided
        Disable();
        if ( _electron != NULL )  { Enable(); std::cout << "ENABLED" << std::endl; }
    }

    BNode* NodeFrom(){ return edge->NodeFrom(); };
    BNode* NodeTo(){ return edge->NodeTo(); };
    
    // this has to go away eventually
    void SetElectron( Electron* _electron ){ electron = _electron; };
    
   
    // changes to be made after this event occurs
    virtual void OnExecute(  State* state, votca::tools::Random2 *RandomVariable ) {
    
        /*
        std::cout << Type() << " of electron " << electron->id() << 
                " from node " << NodeFrom()->id << 
                " to " << NodeTo()->id << std::endl;
        */  
        // if the move is allowed (NodeTo not occupied) then move
        if ( electron->Move(edge) ) {
            // disable old events
            for (auto& event: disabled_events ) {
                event->Disable();
            //    //event->Print("  -- ");
            }
            // update the parent VSSM group
            Event* parent = GetParent();
            parent->ClearSubordinate();
            
            // enable new events
            for (auto& event: enabled_events ) {
                parent->AddSubordinate( event );
                event->SetElectron(electron);
                event->Enable();
                //event->Print("  ++ ");
            }                  
        }        
        else 
        { 
            //Event move is forbidden 
            Disable();

            
            //Include this next part to use the same carrier but find a new event move
            //Not updating the whole VSSM group, just moving up one level 
            //Event* parent = GetParent();
            //if(parent->CumulativeRate()!=0){
            //parent->OnExecute(state, RandomVariable);
            //}
            //else{ return;}
            
        }    
    };

    // creates a vector of electron transfer events for a specific node and electron
    void CreateEvents( std::vector< ElectronTransferSnail* >* events, BNode* node, Electron* electron, bool status ) {           
            
        for (BNode::EdgeIterator it_edge = node->EdgesBegin() ; it_edge != node->EdgesEnd(); ++it_edge) {
                //New event - electron transfer
                Event* _et =  Events().Create( "electron_transfer_snail" );
                _et->SetParent( GetParent() );
                ElectronTransferSnail* et = dynamic_cast<ElectronTransferSnail*>(_et);
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
            ElectronTransferSnail* ct_transfer = dynamic_cast<ElectronTransferSnail*>(event);
            enabled_events.push_back(ct_transfer);
        }
    }

    void AddDisableOnExecute( std::vector< Event* >* events ) {
        for (auto& event: *events ) {
            ElectronTransferSnail* ct_transfer = dynamic_cast<ElectronTransferSnail*>(event);
            disabled_events.push_back(ct_transfer);
        }
    }

    virtual void Print(std::string offset="") {
        std::cout << offset << Type();
        if ( Enabled() ) { std::cout << ": enabled"; } else { std::cout << ": disabled"; };                
        if ( electron == NULL ) { std:: cout << " no carrier "; } else { std::cout << " Carrier: "  << electron->id(); }
        std::cout 
            << " Node "  << edge->NodeFrom()->id << "->" << edge->NodeTo()->id  
            << " Disabled: " << disabled_events.size() 
            << " Enabled: " << enabled_events.size() 
            << " Rate: " << Rate() 
            << " Cumulative rate: " << CumulativeRate() <<  std::endl;
    }
    
private:

    std::vector<ElectronTransferSnail*> disabled_events;
    std::vector<ElectronTransferSnail*> enabled_events;

    // electron to move
    Electron* electron;
    Edge* edge;
    votca::tools::vec distance_pbc;

};


}}
#endif 



