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

    std::string Type(){ return "electron transfer"; } ;
        
    void Initialize( Electron* _electron, Edge* _edge ) {
        electron = _electron;
        edge = _edge;
        distance_pbc = _edge->DistancePBC();
        SetRate( _edge->Rate_electron() );
        //only enable this event if a carrier is provided
        Disable();
        if ( _electron != NULL )  { Enable(); std::cout << "ENABLED" << std::endl; }
        
    }

    BNode* NodeFrom(){ return edge->NodeFrom(); };
    BNode* NodeTo(){ return edge->NodeTo(); };
    
    // this has to go away eventually
    void SetElectron( Electron* _electron ){ electron = _electron; };
      
    // changes to be made after this event occurs
    virtual void OnExecute(  State* state, votca::tools::Random2 *RandomVariable ) {

        if ( electron->Move(edge) == true ) {

            // disable old events
            for (auto& event: disabled_events ) {   
                event->Disable();
                //the event has to be cleared as being unavailable, the flag is removed but the event remains disabled
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
            for (auto& event: events_to_check) {

                //std::cout << " Events to check: " << event->NodeFrom()->id << "->" << event->NodeTo()->id << std::endl;
                //if (parent->Enabled() && event->UnivailableEvent()==true ){
                if (event->UnivailableEvent()==true){
                    //std::cout << " Unavailable events to check: " << event->NodeFrom()->id << "->" << event->NodeTo()->id << std::endl;

                    //if a previous unavailable event is now available (no longer occupied) - Enable it
                    std::vector<BNode*>::iterator it_to   = electron->NodeOccupation ( event->NodeTo() ) ;
                    //std::vector<BNode*>::iterator it_from   = electron->NodeOccupation ( event->NodeFrom() ) ;
                    
                    //if node to is free and node from has a carrier 
                    if ( it_to == electron->e_occupiedNodes.end() ) {
                        event->Enable();
                        event->Available();
                        //std::cout << "***********"  << std::endl;
                        //std::cout << "Event previously unavailable, now available: " << event->NodeFrom()->id << "->" << event->NodeTo()->id << " Rate: " << event->Rate() << std::endl;
                        //std::cout << "***********"  << std::endl;
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
    
    void AddEnableOnExecute( std::vector< Event* >* events ) {
        for (auto& event: *events ) {
            ElectronTransfer* et_transfer = dynamic_cast<ElectronTransfer*>(event);
            enabled_events.push_back(et_transfer);
        }
    }
        
    void AddDisableOnExecute( std::vector< Event* >* events ) {
        for (auto& event: *events ) {
            ElectronTransfer* et_transfer = dynamic_cast<ElectronTransfer*>(event);
            disabled_events.push_back(et_transfer);
        }
    }
     
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