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

#ifndef __VOTCA_KMC_VSSM2_NODES_H_
#define __VOTCA_KMC_VSSM2_NODES_H_

#include <unordered_map>
#include <time.h>
#include <votca/kmc/algorithm.h>
#include "events/carrier_escape.h"
#include <votca/kmc/bnode.h>
#include "events/electron_transfer.h"

//* Two-level VSSM algorithm with nodes at the top level and reactions at the bottom level
//          head
//        /  |  \
//  node_1      node_n   
//  /  |  \
// move_1  move_k
//*

namespace votca { namespace kmc {
  
class VSSM2_NODES : public Algorithm {
    
public:
    
void Initialize ( State* _state, Graph* _graph ) { 
    
    state = _state;
    graph = _graph;
    
    // Map of charge transfer events associated with a particular node
    std::unordered_map< BNode*, std::vector<Event*> > charge_transfer_map;
    
    // Vector of all charge transfer events
    std::vector<ElectronTransfer*> ct_events;
 
    for (Graph::iterator it_node = _graph->nodes_begin(); it_node != _graph->nodes_end(); ++it_node) {
        
        BNode* node_from = *it_node;
        
        // create a new key with an empty vector
        charge_transfer_map.emplace(node_from, vector<Event*>() );
        
        // Loop over all neighbours (edges) of the node 
        for (BNode::EdgeIterator it_edge = node_from->EdgesBegin(); it_edge != node_from->EdgesEnd(); ++it_edge) {

            // For every edge create an event of type transfer
            Event* event_move = Events().Create("electron_transfer");            
            ElectronTransfer* electron_transfer = dynamic_cast<ElectronTransfer*> (event_move);
            electron_transfer->Initialize(NULL, *it_edge);
                        
            // Add a list of charge transfer events to the map, indexed by a node pointer
            charge_transfer_map.at(node_from).push_back(event_move);
            
            // Add an event to the charge transfer events
            ct_events.push_back(electron_transfer);
        }
    }
  
    // for every event, add a list of "events-to-enable" after OnExecute
    // and a list of "events-to-disable" after OnExecute
    for (auto& event: ct_events ) {
        BNode* node_from = event->NodeFrom();
        BNode* node_to = event->NodeTo();
        
        std::vector<Event*> events_to_disable = charge_transfer_map.at(node_from);
        std::vector<Event*> events_to_enable = charge_transfer_map.at(node_to);
        
        event->AddDisableOnExecute(&events_to_disable);
        event->AddEnableOnExecute(&events_to_enable);
    }

    // for every event associated with node_from - create a list of events to check after onExecute
    // Events to check are every event for each node_to
    for (auto& event: ct_events ) {
        
        BNode* node_from = event->NodeFrom();
        std::vector<Event*> ct_events = charge_transfer_map.at(node_from);
        
        for (std::vector<Event*>::iterator it_event = ct_events.begin(); it_event != ct_events.end(); ++it_event) {
            
            Event* event_move = *it_event;
            ElectronTransfer* electron_transfer = dynamic_cast<ElectronTransfer*> (event_move);
            
            BNode* node_to = electron_transfer->NodeTo();
            std::vector<Event*> events_to_check = charge_transfer_map.at(node_to);
            event->CheckEventsOnExecute(&events_to_check);
        }
    }
    
    // organise events in a tree;
    // first level VSSM events (escape event for each carrier))
    for (State::iterator carrier = _state->begin(); carrier != _state->end(); ++carrier) {
        //std::cout << "Adding escape event for carrier " << (*carrier)->Type() << ", id " << (*carrier)->id() << std::endl;

        // create the carrier escape event (leaving the node)
        Event* event_escape = Events().Create("carrier_escape");
        CarrierEscape* carrier_escape = dynamic_cast<CarrierEscape*> (event_escape);
        carrier_escape->Initialize((*carrier));
        head_event.AddSubordinate( event_escape );
        //std::cout << "  parent of " << carrier_escape->Type() << " is " << carrier_escape->GetParent()->Type() << std::endl;

        BNode* node_from = (*carrier)->GetNode();

        std::vector<Event*> ct_events = charge_transfer_map.at(node_from);
                
        // Add move events from the map 
        for (std::vector<Event*>::iterator it_event = ct_events.begin(); it_event != ct_events.end(); ++it_event) {

            // New event - electron transfer
            Event* event_move = *it_event;
            ElectronTransfer* electron_transfer = dynamic_cast<ElectronTransfer*> (event_move);
            Electron* electron = dynamic_cast<Electron*> ((*carrier));
            
            electron_transfer->SetElectron( electron );
            electron_transfer->Enable();
            // add a subordinate event
            carrier_escape->AddSubordinate( event_move );            
            
        }           
    }
    head_event.Enable();

}

void Run( double runtime, int nsteps, int seed, int nelectrons, int nholes, string trajectoryfile, double outtime, double fieldX, double fieldY, double fieldZ) {

    votca::tools::Random2 RandomVariable;

    std::cout << std::endl << "Starting the KMC loop" << std::endl;
   
    clock_t begin = clock();

    // Initialise random number generator
    srand(seed);
    RandomVariable.init(rand(), rand(), rand(), rand());
    
    double time = 0.0;
    int step = 0;
    double trajout = 0.0;
    
    if ( runtime != 0 && nsteps == 0 ){ 
        runtime = runtime;
        std::cout << "Specified runtime (s): " << runtime << std::endl; 
    }
    else if ( nsteps != 0 && runtime == 0 ){ 
        nsteps = nsteps; 
        std::cout << "Specified number of simulation steps: " << nsteps << std::endl;
    }
    else if ( runtime !=0 && nsteps !=0 ){ 
        nsteps = nsteps; runtime = 0.0; 
        std::cout << "Specified both the number of simulation steps and the runtime -> number of steps will be used. " << std::endl; 
        std::cout << "Specified number of simulation steps: " << nsteps << std::endl;
    }
    
    state->Trajectory_create(trajectoryfile);
    state->Trajectory_write(trajout, trajectoryfile);
    
    trajout = outtime;
    
    while ( step < nsteps || time < runtime ){  
        
        //head_event.Print();
             
        head_event.OnExecute(state, &RandomVariable );         
        double u = 1.0 - RandomVariable.rand_uniform();
        while(u == 0.0){ u = 1.0 - RandomVariable.rand_uniform();}
        double elapsed_time = -1.0 / head_event.CumulativeRate() * log(u);
        state->AdvanceClock(elapsed_time);
        time += elapsed_time;
        step++;
        //std::cout << "Time: " << time << std::endl;
        
        if (outtime != 0 && trajout < time )
        { 
            state->Trajectory_write(trajout, trajectoryfile);
            trajout = time + outtime;
        }

    }
    state->Print_properties(nelectrons, nholes, fieldX, fieldY, fieldZ);
    clock_t end = clock();    
    printf("Elapsed: %f seconds after %i steps \n", (double)(end - begin) / CLOCKS_PER_SEC, step);
     
}

private:
    
    // head event, contains all escape events
    CarrierEscape head_event;
    // logger : move logger.h to tools
    // Logger log;   
};
    
}}



#endif

