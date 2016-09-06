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

#ifndef __VOTCA_KMC_GRAPH_H_
#define __VOTCA_KMC_GRAPH_H_

#include <votca/kmc/bnode.h>

namespace votca { namespace kmc {
  
class Graph {
public:
    
    void Load(std::string filename);    
    void Print();
    
    // add a node to the vector of Nodes
    BNode* AddNode() {
        BNode *node = new BNode();
        nodes.push_back( node );
        return node;
    };
    
    BNode* GetNode( int id ) {
        std::vector< BNode* >::iterator node = nodes.begin() ;
        while ( (*node)->id != id ) node++ ;
        return *node;
    };
       
private:
    std::vector< BNode* > nodes;
    //std::unordered_map< int, BNode* > node_ids;
    
};


void Graph::Load(std::string filename) {
    
    std::cout << "Loading the graph from " << filename << std::endl;
    
    // initialising the database file
    votca::tools::Database db;
    db.Open( filename );
    
    votca::tools::Statement *stmt = db.Prepare("SELECT id-1, posX, posY, posZ FROM segments;");

    while (stmt->Step() != SQLITE_DONE)
    {
       
        BNode *node = AddNode();

        //std::cout << "id: " << stmt->Column<int>(0) << "; ";
        int id = stmt->Column<int>(0);
        node->id = id+1;
        
        // position in nm
        double x = stmt->Column<double>(1);
        double y = stmt->Column<double>(2);
        double z = stmt->Column<double>(3);
    
        //cout << "position: " <<  x << " " << y << " " << z << endl;  
        
        myvec position = myvec(x, y, z); 
        node->position = position;
        
        //node->PrintNode();
        
    }
    
    delete stmt;
    
    //List of neighbours (from neighbour list for charge transfer - state.sql "pairs" table)  
    stmt = db.Prepare("SELECT seg1, seg2 FROM pairs;");
    while (stmt->Step() != SQLITE_DONE)     
    {           
        int seg1 = stmt->Column<int>(0);
        int seg2 = stmt->Column<int>(1);

        //Get the nodes for seg 1 and 2
        BNode* node1 = GetNode( seg1 );
        BNode* node2 = GetNode( seg2 );
        
        // make sure no duplicates are added
        node1->AddNeighbor( node2 );
        node2->AddNeighbor( node1 );
         
    }
    delete stmt;

}

    void Graph::Print(){
    
        for (std::vector< BNode* >::iterator node = nodes.begin() ; node != nodes.end(); ++node) {
            (*node)->PrintNode();
        }
    };
 
    
}}



#endif

