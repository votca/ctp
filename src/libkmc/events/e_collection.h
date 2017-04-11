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

#ifndef __VOTCA_KMC_ElectronCollection_H
#define __VOTCA_KMC_ElectronCollection_H

#include <votca/kmc/event.h>
#include <votca/kmc/edge.h>
#include "carriers/electron.h"

namespace votca { namespace kmc {
    
class ElectronCollection : public Event {
    
public:

    std::string Type(){ return "electron collection"; } ;
    
    void Initialize( Electron* _electron, Edge* _edge ) {
        electron = _electron;
        edge = _edge;
        distance_pbc = _edge->DistancePBC();
        SetRate( _edge->Rate() );
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
            
            std::cout << "TEST" << std::endl;
            
            std::cout << "Electron collected" << std::endl;       
        
            // disable old events
            for (auto& event: disabled_events ) {   
                event->Disable();     
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
            ElectronInjection* electron_injection = dynamic_cast<ElectronInjection*>(event);
            enabled_events.push_back(electron_injection);
        }
    }

    void AddDisableOnExecute( std::vector< Event* >* events ) {
        for (auto& event: *events ) {
            ElectronCollection* electron_collection = dynamic_cast<ElectronCollection*>(event);
            disabled_events.push_back(electron_collection);
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

    std::vector<ElectronCollection*> disabled_events;
    std::vector<ElectronInjection*> enabled_events;
    
    // electron to inject
    Electron* electron;
    Edge* edge;
    votca::tools::vec distance_pbc;
    
};


}}
#endif 