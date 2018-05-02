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
#include "../carriers/electron.h"

namespace votca { namespace kmc {
    
class ElectronTransfer : public Event {
    
public:

    std::string Type(){ return "electron transfer"; } ;
        
    void Initialize( Electron* _electron, Edge* _edge ) {
        electron = _electron;
        edge = _edge;
        distance_pbc = _edge->DistancePBC();
        SetRate( _edge->Rate_electron() );
        //only enable this event if a carrier is provided - parent event (carrier escape) is enabled)
        //default is disable 
        Disable();
        if ( _electron != NULL )  { Enable(); std::cout << "ENABLED" << std::endl; }
        
    }

    //node where the carrier starts
    BNode* NodeFrom(){ return edge->NodeFrom(); };
    // node where the carrier hops to
    BNode* NodeTo(){ return edge->NodeTo(); };

    void SetElectron( Electron* _electron ){ electron = _electron; };
      
    // changes to be made after this event occurs
    virtual void OnExecute(  State* state, votca::tools::Random2 *RandomVariable ) {

        if ( electron->Move(edge) == true ) {

            // disable old events
            for (auto& event: disabled_events ) {   
                event->Disable();
                //the event has to be cleared as being unavailable, the flag is removed but the event remains disabled
                //events can only be unavailable if a carrier is present
                event->Available();
            }
            
            // update the parent VSSM group
            Event* parent = GetParent();
            parent->ClearSubordinate();
            
            // enable new events
            for (auto& event: enabled_events ) {
                parent->AddSubordinate( event );
                event->SetElectron(electron);
                event->Enable();
            } 
            
            //check all the events connected to the previous node (node_from)
            //This checks if any connected events have now became available 
            //eg. node i and j are occupied, event i->j is disabled, now carrier moves and j becomes free (all events j-> are disabled) 
            //but there still has to be a check that event i->j is now available to i 
            for (auto& event: events_to_check) {
                
                if (event->UnivailableEvent()==true){                    

                    //iterate over all previous nodes to
                    std::vector<BNode*>::iterator it_to   = electron->NodeOccupation ( event->NodeTo() ) ;
                    
                    //if node to is free and node from has a carrier 
                    if ( it_to == electron->e_occupiedNodes.end() ) {
                        //if a previous unavailable event is now available (no longer occupied node to) - Enable it
                        event->Enable();
                        //remove the unavailable flag
                        event->Available();
                    }
                } 
            }
   
        }        
        else 
        { 
            //Event move is unavailable - already occupied           
            Unavailable();
            //Disable this event
            Disable();            
        }         
    };
    
    //events that will be enabled after hop
    void AddEnableOnExecute( std::vector< Event* >* events ) {
        for (auto& event: *events ) {
            ElectronTransfer* et_transfer = dynamic_cast<ElectronTransfer*>(event);
            enabled_events.push_back(et_transfer);
        }
    }
     
    //events that will be diabled after a hop
    void AddDisableOnExecute( std::vector< Event* >* events ) {
        for (auto& event: *events ) {
            ElectronTransfer* et_transfer = dynamic_cast<ElectronTransfer*>(event);
            disabled_events.push_back(et_transfer);
        }
    }
     
    //events that need to be checked after a hop
    void CheckEventsOnExecute( std::vector<Event*>* events){
        for (auto& event: *events){
            ElectronTransfer* et_transfer = dynamic_cast<ElectronTransfer*>(event);
            events_to_check.push_back(et_transfer);
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

    std::vector<ElectronTransfer*> disabled_events;
    std::vector<ElectronTransfer*> enabled_events;
    std::vector<ElectronTransfer*> events_to_check;
    
    // electron to move
    Electron* electron;
    Edge* edge;
    votca::tools::vec distance_pbc;
    
};


}}
#endif 