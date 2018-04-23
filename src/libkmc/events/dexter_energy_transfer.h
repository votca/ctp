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

#ifndef __VOTCA_KMC_DEXTERENERGYTRANSFER_H
#define __VOTCA_KMC_DEXTERENERGYTRANSFER_H

#include <votca/kmc/event.h>
#include <votca/kmc/edge.h>
#include "../carriers/energy.h"

namespace votca { namespace kmc {
    
class DexterEnergyTransfer : public Event {
    
public:

    std::string Type(){ return "dexter energy transfer"; } ;
        
    void Initialize( Energy* _energy, Edge* _edge ) {
        energy = _energy;
        edge = _edge;
        distance_pbc = _edge->DistancePBC();
        SetRate( _edge->Rate_dexter_energy() );
        //only enable this event if a carrier is provided
        Disable();
        if ( _energy != NULL )  { Enable(); std::cout << "ENABLED" << std::endl; }
        
    }

    BNode* NodeFrom(){ return edge->NodeFrom(); };
    BNode* NodeTo(){ return edge->NodeTo(); };
    
    // this has to go away eventually
    void SetEnergy( Energy* _energy ){ energy = _energy; };
      
    // changes to be made after this event occurs
    virtual void OnExecute(  State* state, votca::tools::Random2 *RandomVariable ) {

        if ( energy->Move(edge) == true ) {

            if (energy->move_type=="dexter"){state->CountEvent("dexter energy transfer");}
            else if (energy->move_type=="inject"){state->CountEvent("inject");}
            else if (energy->move_type=="collect"){state->CountEvent("dexter collect");}
            
            // disable old events
            for (auto& event: disabled_events ) {   
                event->Disable();     
            }
            //check all the events connected to the previous node (node_from)
            for (auto& event: events_to_check) {

                //std::cout << " Events to check: " << event->NodeFrom()->id << "->" << event->NodeTo()->id << std::endl;
                if (event->UnivailableEvent()==true){
                    //std::cout << " Unavailable events to check: " << event->NodeFrom()->id << "->" << event->NodeTo()->id << std::endl;

                    //if a previous unavailable event is now available (no longer occupied) - Enable it
                    std::vector<BNode*>::iterator it_to   = energy->NodeOccupation ( event->NodeTo() ) ;
                    if ( it_to == energy->energy_occupiedNodes.end() ) {
                        event->Enable();
                    }
                } 
            }
      
            // update the parent VSSM group
            Event* parent = GetParent();
            parent->ClearSubordinate();
            
            // enable new events
            for (auto& event: enabled_events ) {
                parent->AddSubordinate( event );
                event->SetEnergy(energy);
                event->Enable();
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
            DexterEnergyTransfer* dexter_transfer = dynamic_cast<DexterEnergyTransfer*>(event);
            enabled_events.push_back(dexter_transfer);
        }
    }
        
    void AddDisableOnExecute( std::vector< Event* >* events ) {
        for (auto& event: *events ) {
            DexterEnergyTransfer* dexter_transfer = dynamic_cast<DexterEnergyTransfer*>(event);
            disabled_events.push_back(dexter_transfer);
        }
    }
     
    void CheckEventsOnExecute( std::vector<Event*>* events){
        for (auto& event: *events){
            DexterEnergyTransfer* dexter_transfer = dynamic_cast<DexterEnergyTransfer*>(event);
            events_to_check.push_back(dexter_transfer);
        }  
    }
    
    virtual void Print(std::string offset="") {
        std::cout << offset << Type();
        if ( Enabled() ) { std::cout << ": enabled"; } else { std::cout << ": disabled"; };                
        if ( energy == NULL ) { std:: cout << " no carrier "; } else { std::cout << " Carrier: "  << energy->id(); }
        std::cout 
            << " Node "  << edge->NodeFrom()->id << "->" << edge->NodeTo()->id  
            << " Disabled: " << disabled_events.size() 
            << " Enabled: " << enabled_events.size() 
            << " Rate: " << Rate() 
            << " Cumulative rate: " << CumulativeRate() <<  std::endl;
    }
        
private:

    std::vector<DexterEnergyTransfer*> disabled_events;
    std::vector<DexterEnergyTransfer*> enabled_events;
    std::vector<DexterEnergyTransfer*> events_to_check;
    
    // energy to move
    Energy* energy;
    Edge* edge;
    votca::tools::vec distance_pbc;
    
};


}}
#endif
