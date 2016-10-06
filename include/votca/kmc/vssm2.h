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

#ifndef __VOTCA_KMC_VSSM2_H_
#define __VOTCA_KMC_VSSM2_H_

#include <votca/ctp/logger.h>
#include <votca/kmc/algorithm.h>
#include "events/carrier_escape.h"

//* Two-level VSSM algorithm with carriers at the top level and transfer reactions at the bottom level
//          head
//        /  |  \
//  escape_1      escape_n (one per carrier)
//  /  |  \
// move_1  move_k
//*

namespace votca { namespace kmc {
  
class VSSM2 : public Algorithm {
    
public:

void Initialize ( std::vector<Event*> events, State* _state, Graph* graph ) {
    
    state = _state;
    
    // first level VSSM events (escape event for each carrier))
    for (State::iterator carrier = state->begin(); carrier != state->end(); ++carrier) {
        std::cout << "Adding escape event for carrier " << (*carrier)->Type() << ", id " << (*carrier)->id() << std::endl;

        // create the carrier escape event (leaving the node)
        Event* event_escape = Events().Create("carrier_escape");
        CarrierEscape* carrier_escape = dynamic_cast<CarrierEscape*> (event_escape);
        carrier_escape->Initialize((*carrier));
        head_event.AddSubordinate( event_escape );
        std::cout << "  parent of " << carrier_escape->Type() << " is " << carrier_escape->GetParent()->Type() << std::endl;

        BNode* node_from = (*carrier)->GetNode();

        // initialize move events - hole, electron, exciton transfer
        for (BNode::iterator node_to = node_from->begin(); node_to != node_from->end(); ++node_to) {

            //New event - electron transfer
            Event* event_move = Events().Create("electron_transfer");
            ElectronTransfer* electron_transfer = dynamic_cast<ElectronTransfer*> (event_move);
            Electron* electron = dynamic_cast<Electron*> ((*carrier));
            
            //// rewrite as a loop over links
            ////electron_transfer->Initialize(electron, node_from, (*node_to), 1.0);
            
            // add a subordinate event
            carrier_escape->AddSubordinate( event_move );
            
        }
        
        // evaluate the escape rate (sum of all enabled subordinate events)
        carrier_escape->CumulativeRate(); 
        std::cout << "Total escape rate " <<  carrier_escape->CumulativeRate() << std::endl;

    }

}
    
void Run( double runtime ) {

    votca::tools::Random2 RandomVariable;

    std::cout << "I am running" << std::endl;

    // Initialise random number generator
    int _seed = 0;
    srand(_seed);
    RandomVariable.init(rand(), rand(), rand(), rand());

    runtime = 1000.0;
    double time = 0.0;
    // execute the head VSSM event and update time
    while ( time <= runtime ) {
        head_event.OnExecute(state, &RandomVariable ); 
        double elapsed_time = 1./head_event.CumulativeRate();
        state->AdvanceClock(elapsed_time);
        state->Print();
        time += elapsed_time;
        std::cout << "Time: " << time << std::endl;
    }
    
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

