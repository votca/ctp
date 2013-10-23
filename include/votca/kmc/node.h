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

#ifndef __VOTCA_KMC_NODE_H_
#define	__VOTCA_KMC_NODE_H_

#include <vector>
#include <votca/tools/vec.h>
#include "carrier.h"

typedef votca::tools::vec myvec;

namespace votca { namespace kmc {
  
using namespace std;


class Node {
    
public:

    void setPair(Node* pairing_node) {pairing_nodes.push_back(pairing_node);}
    void setStaticeventinfo(Node* pairnode, myvec dr, double rate12e, double rate12h, double Jeff2e, double Jeff2h, double reorg_oute, double reorg_outh);    
    
    struct Static_event_info {
        Node* pairnode;
        myvec distance; //distance vector from start to destination node
        double rate12e;
        double rate12h;
        double Jeff2e;
        double Jeff2h;
        double reorg_oute;
        double reorg_outh;
    };     

    int node_ID;
    myvec node_position;
    vector<Node*> pairing_nodes;
    vector<Static_event_info> static_event_info;
 
    //static energies
    double reorg_intorig_hole;
    double reorg_intorig_electron;
    double reorg_intdest_hole;
    double reorg_intdest_electron;
        
    double eAnion;
    double eNeutral;
    double eCation;
        
    double internal_energy_electron;
    double internal_energy_hole;
        
    double static_electron_node_energy;
    double static_hole_node_energy;

    
};

void Node::setStaticeventinfo(Node* pairnode, myvec dr, double rate12e, double rate12h, double Jeff2e, double Jeff2h, double reorg_oute, double reorg_outh) {
    Static_event_info newStatic;
    newStatic.pairnode = pairnode;
    newStatic.distance = dr;
    newStatic.rate12e = rate12e;
    newStatic.rate12h = rate12h;
    newStatic.Jeff2e = Jeff2e;
    newStatic.Jeff2h = Jeff2h;
    newStatic.reorg_oute = reorg_oute;
    newStatic.reorg_outh = reorg_outh;
}       
        
}} 

#endif

