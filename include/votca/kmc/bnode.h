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
#include <votca/kmc/edge.h>
#include <votca/kmc/bnode.h>

class Edge;

namespace votca { namespace kmc {
        
class BNode {
    
public:

    typedef std::vector< Edge* >::iterator EdgeIterator;
    typedef const std::vector< Edge* >::const_iterator const_EdgeIterator;
    
    EdgeIterator EdgesBegin() { return edges.begin(); }
    EdgeIterator EdgesEnd() { return edges.end(); }
    
    
   void AddEdge( Edge* edge ) {
        edges.push_back(edge);
    };
       
    void PrintNode(){
        std::cout << "Neighbour list of node " << id << ": "; //Node ID followed by list of neighbouring nodes
        for (std::vector< Edge* >::iterator edge = edges.begin() ; edge != edges.end(); ++edge) {
            std::cout << (*edge)->NodeTo()->id << " ";
        }
        std::cout << std::endl;      
    };
    
    votca::tools::vec position;

    int id;
    
private:
    
    std::vector< Edge* > edges;
     
};



        
}} 

#endif

