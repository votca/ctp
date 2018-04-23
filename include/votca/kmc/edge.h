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

#ifndef __VOTCA_KMC_EDGE_H_
#define	__VOTCA_KMC_EDGE_H_


#include <votca/tools/vec.h>
#include <votca/kmc/bnode.h>

namespace votca { namespace kmc {

class BNode;    
    
class Edge {

public:
    
    Edge(BNode* _node_from, BNode* _node_to, votca::tools::vec _distance_pbc, double _rate_e = 0.0, 
            double _rate_h = 0.0, double _rate_dexter = 0.0, double _rate_forster = 0.0, 
            double _rate_inject = 0.00, double _rate_collect = 0.0, double _rate_return = 0.0) 
    {
        node_from = _node_from;
        node_to = _node_to;
        distance_pbc = _distance_pbc;
        rate_electron = _rate_e;
        rate_hole = _rate_h;
        rate_dexter_et = _rate_dexter;
        rate_forster_et = _rate_forster;
        rate_inject = _rate_inject;
        rate_collect = _rate_collect;
        rate_return = _rate_return;
    }
    
    
    BNode* NodeFrom() { return node_from; }
    BNode* NodeTo() { return node_to; }
    votca::tools::vec DistancePBC() {return distance_pbc; }
    double Rate_electron() { return rate_electron; }
    double Rate_hole() { return rate_hole; }
    double Rate_dexter_energy() { return rate_dexter_et; }
    double Rate_forster_energy() { return rate_forster_et; }
    double Rate_inject() { return rate_inject; }
    double Rate_collect() { return rate_collect; }
    double Rate_return() { return rate_return;}
    
private:
        
   ~Edge() {};
   
    BNode* node_from;
    BNode* node_to;
    
    votca::tools::vec distance_pbc;

    double rate_electron;
    double rate_hole;
    double rate_dexter_et;
    double rate_forster_et;
    double rate_inject;
    double rate_collect;
    double rate_return;
    
};
 
}}
 
#endif

