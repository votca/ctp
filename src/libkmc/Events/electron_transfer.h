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
#include <votca/kmc/carrier.h>
#include "carriers/electron.h"

namespace votca { namespace kmc {
    
class Electrontransfer : public Event {
    
public:
    
    std::string Type(){ return "electron transfer"; } ;
    
    // electron to move
    Carrier* electron;
    // Move from this node
    BNode* node_from;
    // Move to this node
    BNode* node_to;
    //rate of electron transfer
    double rate;
    
    void AddElectron( Carrier* _electron );
    void SetOrigin( BNode* _node );
    void SetDestination( BNode* _node);
   
    // electron transfer rate
    void SetRate( double rate );

    virtual void OnExecute(  State* state ) {
    
        std::cout << "Moving an " << electron->Type() << " " << electron->id() << 
                " from node " <<  node_from->id << 
                " to node "   <<  node_to->id << std::endl;
    };
    
private:
    
    std::vector< BNode* > neighbours;
 
};

inline void Electrontransfer::AddElectron( Carrier* _electron )
{
    electron = _electron;
}

inline void Electrontransfer::SetOrigin( BNode* _node )
{
    node_from = _node;   
    std::cout << "Electron " << electron->id() << " sits on node: " << node_from->id;
}

inline void Electrontransfer::SetDestination( BNode* _node )
{
    node_to = _node;   
    std::cout << "Electron " << electron->id() << " goes to node: " << node_to->id;
}

inline void Electrontransfer::SetRate( double _rate )
{
    rate = _rate;
}

}}
#endif 
