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
#define __VOTCA_KMC_VSSM2__NODES_H_

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

//Need to run over all nodes - find occupied nodes (where carriers sit) - loop over occupied nodes/carriers    
    
    // Map of charge transfer events associated with a particular node
    std::unordered_map< BNode*, std::vector<Event*> > charge_transfer_map;
     
    // Vector of all charge transfer events
    std::vector<ElectronTransfer*> ct_events;
    
    // For every node create an escape event and attach it to the head event
    for (Graph::iterator it_node = _graph->nodes_begin(); it_node != _graph->nodes_end(); ++it_node) {
        
        BNode* node_from = *it_node;
        
        std::cout << "Adding escape event for node " << node_from->id << std::endl;
        
        // create the carrier escape event (leaving the node)
        Event* event_escape = Events().Create("carrier_escape");
        CarrierEscape* carrier_escape = dynamic_cast<CarrierEscape*> (event_escape);
        
        // Add new event to the head event
        head_event.AddSubordinate( event_escape );
        
        //std::vector<Event*> charge_transfer_node_events;
        //std::cout << "  parent of " << carrier_escape->Type() << " is " << carrier_escape->GetParent()->Type() << std::endl;
        
        //std::vector<Event*> ct_events = charge_transfer_map.at(node_from);

        // create a new key with an empty vector
        charge_transfer_map.emplace(node_from, vector<Event*>() );
        
        Carrier* carrier = state->NodeOccupation( node_from );
        Electron* electron = NULL;
        
        if ( ( carrier != NULL ) && ( carrier->Type() == "electron"  ) ) { 
            electron = dynamic_cast<Electron*> (carrier);
            std::cout << "Found an electron" << std::endl;
            event_escape->Enable();
            head_event.Enable();
        }
        
        
        // Loop over all neighbours (edges) of the node 
        for (BNode::EdgeIterator it_edge = node_from->EdgesBegin(); it_edge != node_from->EdgesEnd(); ++it_edge) {

            // For every edge create an event of type transfer
            Event* event_move = Events().Create("electron_transfer");            
            ElectronTransfer* electron_transfer = dynamic_cast<ElectronTransfer*> (event_move);
            electron_transfer->Initialize(electron, *it_edge);
            
            // add a subordinate event
            event_escape->AddSubordinate( event_move );
            //charge_transfer_node_events.push_back( event_move );
            
            // Add a list of charge transfer events to the map, indexed by a node pointer
            charge_transfer_map.at(node_from).push_back(event_move);
            
            // Add an event to the charge transfer events
            ct_events.push_back(electron_transfer);            
                   
        }
        
        // evaluate the escape rate (sum of all enabled subordinate events)
        event_escape->CumulativeRate(); 
        std::cout << "Total escape rate " <<  event_escape->CumulativeRate() << std::endl;
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
       
    head_event.Enable();
    //head_event.Print();
  
}

void Run( double runtime ) {

    votca::tools::Random2 RandomVariable;

    std::cout << "Starting the KMC loop" << std::endl;
    
    clock_t begin = clock();

    // Initialise random number generator
    int _seed = 123456;
    srand(_seed);
    RandomVariable.init(rand(), rand(), rand(), rand());


    double time = 0.0;
    int step = 0;
    int nsteps = 1000000;
    // execute the head VSSM event and update time
    //while ( time <= runtime ) {
    while ( step < nsteps ) {
        //head_event.Print();
        head_event.OnExecute(state, &RandomVariable ); 
        double u = 1.0 - RandomVariable.rand_uniform();
        double elapsed_time = -1.0 / head_event.CumulativeRate() * log(u);
        state->AdvanceClock(elapsed_time);
        //state->Print();
        time += elapsed_time;
        step++;
        std::cout << "Time: " << time << std::endl;
    }

    state->Print();
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

