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

#include <votca/kmc/algorithm.h>
#include "Events/carrier_escape.h"
#include <time.h>
#include <votca/kmc/bnode.h>

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

void Initialize ( std::vector<Event*> events, std::vector<BNode*> nodes, Graph* graph, State* _state) {
    
     state = _state;
    
    // first level VSSM - list of nodes (escape events from the node)
    for (Graph::iterator node = Graph->nodes_begin(); node != Graph->nodes_end(); ++node) {
        std::cout << "Node id: " << Graph->GetNode()->id << std::endl;
        
        //Initialise carriers - which can sit on the node?
        //Node -> Carrier -> escape event -> move event
        
        //Initialise the escape events - leaving the node
        Event* event_escape = Events().Create("carrier_escape");
        CarrierEscape* carrier_escape = dynamic_cast<CarrierEscape*> (event_escape);
        carrier_escape->Initialize((*carrier));
        head_event.AddSubordinate( event_escape );
        std::cout << "  parent of " << carrier_escape->Type() << " is " << carrier_escape->GetParent()->Type() << std::endl;
        
        
        BNode* node_from = node;

            //initialise move events - hole, electron, exciton transfer 
            for (BNode::EdgeIterator it_edge = node_from->EdgesBegin(); it_edge != node_from->EdgesEnd(); ++it_edge) {

            //New event - electron transfer
            Event* event_move = Events().Create("electron_transfer");
            ElectronTransfer* electron_transfer = dynamic_cast<ElectronTransfer*> (event_move);
            Electron* electron = dynamic_cast<Electron*> (Carrier*);
            
            electron_transfer->Initialize(electron, *it_edge);
            
            // add a subordinate event
            event_escape->AddSubordinate( event_move );
            
            }
        
        // evaluate the escape rate (sum of all enabled subordinate events)
        event_escape->CumulativeRate(); 
        std::cout << "Total escape rate " <<  event_escape->CumulativeRate() << std::endl;

    }

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
        state->AdvanceClock(elapsed_time);
        //state->Print();
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

