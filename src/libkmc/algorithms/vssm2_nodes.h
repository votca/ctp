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

        // Map of charge transfer events associated with a particular node
        std::unordered_map< BNode*, std::vector<Event*> > charge_transfer_map;
     
    // For every node create an escape event and attach it to the head event)
    for (Graph::iterator it_node = _graph->nodes_begin(); it_node != _graph->nodes_end(); ++it_node) {
        
        BNode* node_from = *it_node;
        std::cout << "Node id: " << node_from->id << " ";
        
        // Create the escape events - leaving the node
        Event* event_escape = Events().Create("carrier_escape");
        CarrierEscape* carrier_escape = dynamic_cast<CarrierEscape*> (event_escape);

        // Add new event to the head event
        head_event.AddSubordinate( event_escape );
        
        std::vector<Event*> charge_transfer_node_events;
        
        
        
        // Loop over all neighbours (edges) of the node 
        for (BNode::EdgeIterator it_edge = node_from->EdgesBegin(); it_edge != node_from->EdgesEnd(); ++it_edge) {

            // For every edge create an event of type transfer
            Event* event_move = Events().Create("electron_transfer");            
            ElectronTransfer* electron_transfer = dynamic_cast<ElectronTransfer*> (event_move);
            electron_transfer->Initialize(NULL, *it_edge);
            
            // add a subordinate event
            event_escape->AddSubordinate( event_move );
            charge_transfer_node_events.push_back( event_move ); 

            // Add a list of charge transfer events to the map, indexed by a node pointer
                charge_transfer_map.at(node_from).push_back(event_move);
            
        }
        
        // evaluate the escape rate (sum of all enabled subordinate events)
        event_escape->CumulativeRate(); 
        std::cout << "Total escape rate " <<  event_escape->CumulativeRate() << std::endl;

    }
    
        //        Carrier* carrier = state->NodeOccupation( node_from );
        //    Electron* electron = NULL;
        //    if ( carrier != NULL ) { 
        //        electron = dynamic_cast<Electron*> (carrier);
        //        std::cout << "Found a carrier" << std::endl;
        //        event_escape->Enable();
        //        head_event.Enable();
        //    }

}
    
void Run( double runtime ) {

    votca::tools::Random2 RandomVariable;

    std::cout << "Starting the KMC loop" << std::endl;
    
    clock_t begin = clock();

    // Initialise random number generator
    int _seed = 0;
    srand(_seed);
    RandomVariable.init(rand(), rand(), rand(), rand());

    runtime = 1E-4;
    double maxstep = 1000000;
    double time = 0.0;
    int step = 0;
    
    // execute the head VSSM event and update time
    while ( ( time <= runtime ) && ( step <= 1000000 ) ) {
        head_event.OnExecute(state, &RandomVariable ); 
        double elapsed_time = 1./head_event.CumulativeRate();

        //std::cout << elapsed_time;
        
        state->AdvanceClock(elapsed_time);
        state->Print();
        time += elapsed_time;
        step++;
        //std::cout << "Time: " << time << std::endl;
    }

    state->Print();
    clock_t end = clock();    
    printf("Elapsed: %f seconds\n", (double)(end - begin) / CLOCKS_PER_SEC);
        
}

private:

    // head event, contains all escape events
    CarrierEscape head_event;
    // logger : move logger.h to tools
    // Logger log;
    
    // not nice to have it here
    State* state;

};
    
}}



#endif

