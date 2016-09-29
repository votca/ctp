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

#include <votca/kmc/algorithm.h>
#include "events/carrier_escape.h"

//* Two-level VSSM algorithm with carriers at the top level and transfer reactions at the bottom level

namespace votca { namespace kmc {
  
class VSSM2 : public Algorithm {
    
public:

void Initialize ( std::vector<Event*> events, State* state, Graph* graph ) {

    // construct the first level VSSM events
    for (State::iterator carrier = state->begin(); carrier != state->end(); ++carrier) {
        std::cout << "Adding escape event for carrier " << (*carrier)->Type() << ", id " << (*carrier)->id() << std::endl;

        Event* event = Events().Create("carrier_escape");
        CarrierEscape* carrier_escape = dynamic_cast<CarrierEscape*> (event);
        carrier_escape->Initialize((*carrier));

        level1.push_back(event);

        BNode* node_from = (*carrier)->GetNode();

        // creates events level to events (charge transfer)
        for (BNode::iterator node_to = node_from->begin(); node_to != node_from->end(); ++node_to) {

            //New event - electron transfer

            Event* event = Events().Create("electron_transfer");
            ElectronTransfer* electron_transfer = dynamic_cast<ElectronTransfer*> (event);

            Electron* electron = dynamic_cast<Electron*> ((*carrier));
            electron_transfer->Initialize(electron, node_from, (*node_to), 0.0);


        }

    }


}
    
void Run( double runtime ) {
    std::cout << "I am running" << std::endl;
}

private:
    std::vector<Event*> level1;
    std::vector<Event*> level2;

};
    
}}



#endif

