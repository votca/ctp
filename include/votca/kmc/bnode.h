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

#ifndef __VOTCA_KMC_BNODE_H_
#define	__VOTCA_KMC_BNODE_H_

#include <votca/tools/vec.h>

namespace votca { namespace kmc {
  
class BNode {
    
public:

    void AddNeighbor( BNode* node ) {
        std::cout << "Node " << id << "; Adding neighbour " << node->id << endl;
        neighbours.push_back(node);
    };
    
    void RemoveNeighbor(){};
    
    void PrintNode(){
        std::cout << "Printing node " << id << " ";
        for (std::vector< BNode* >::iterator node = neighbours.begin() ; node != neighbours.end(); ++node) {
            std::cout << (*node)->id << " ";
        }
        std::cout << std::endl;
    };
    
    int id;
    votca::tools::vec position;
    
private:
    
    std::vector< BNode* > neighbours;
     
};

        
}} 

#endif

