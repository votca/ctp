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

#ifndef __VOTCA_KMC_EVENTS_UPDATE_TREE_H_
#define __VOTCA_KMC_EVENTS_UPDATE_TREE_H_

#include <votca/kmc/bnode.h>
#include "events/carrier_escape.h"
#include "events/electron_transfer.h"
#include "algorithms/vssm2_nodes.h"
#include <unordered_map>



//* Events update tree, with reactions as the leaf nodes
//
//           root
//          /    \
//     level 1 
//     /   \
//   level 2 
//    /   \
//  leaf 
// (event)
//
//*

namespace votca { namespace kmc {
 
//Class for the tree - functions that act on the whole tree (e.g. updating the tree)    
class Event_update_tree {
    
public:
   
   
void Initialize ( State* _state, Graph* _graph ){
    
    VSSM2_NODES->Initialize(_state, _graph);
    
    head_event.AddSubordinate( event_escape );
    
    // iterator over subordinate events
    typedef std::vector< Event* >::iterator iterator;
    typedef const std::vector< Event* >::iterator const_iterator;
    
    iterator begin() { return subordinate.begin(); }
    iterator end() { return subordinate.end(); }    

    for ( Event::iterator event = begin(); event != end(); ++event  ) {
           
        }
     
}
    

private:

    
    CarrierEscape head_event;

};

    
    
    
#endif
