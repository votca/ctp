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

#ifndef __VOTCA_KMC_TERMINALGRAPH_H_
#define __VOTCA_KMC_TERMINALGRAPH_H_

#include <votca/kmc/bnode.h>

namespace votca { namespace kmc {
  
class TerminalGraph {
public:
    
    // iterator over carriers
    typedef std::vector< BNode* >::iterator iterator;
    typedef const std::vector< BNode* >::iterator const_iterator;
    
    iterator nodes_begin() { return nodes.begin(); }
    iterator nodes_end() { return nodes.end(); } 
    int nodes_size() {return nodes.size(); }
    
    iterator injectable_nodes_begin() { return injectable_nodes.begin(); }
    iterator injectable_nodes_end() { return injectable_nodes.end(); } 
    int injectable_nodes_size() {return injectable_nodes.size();}
    
    iterator collection_nodes_begin() { return collection_nodes.begin(); }
    iterator collection_nodes_end() { return collection_nodes.end(); } 
    int collection_nodes_size() {return collection_nodes.size();}
    
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
    
    // node selection from the injectable nodes
    BNode* GetInjectionNode( int id ) {
        std::vector< BNode* >::iterator node = injectable_nodes.begin() ;
        while ( (*node)->id != id ) node++ ;
        return *node;
    };
  
private:

    std::vector< BNode* > nodes;
    std::vector < BNode* > injectable_nodes;
    std::vector < BNode* > collection_nodes;
    std::vector< Edge* > edges;

    
};


void TerminalGraph::Load(std::string filename) {
    
    std::cout << "Loading the graph from " << filename << std::endl;
    
    // initialising the database file
    votca::tools::Database db;
    db.Open( filename );
    
    votca::tools::Statement *stmt = db.Prepare("SELECT id-1, posX, posY, posZ FROM segments;");

    while (stmt->Step() != SQLITE_DONE)
    {
        BNode *node = AddNode();
        int id = stmt->Column<int>(0);
        node->id = id+1;
        
        // position in nm
        double x = stmt->Column<double>(1);
        double y = stmt->Column<double>(2);
        double z = stmt->Column<double>(3);       
   
        myvec position = myvec(x, y, z); 
        node->position = position;
        
        //Only add an injectable node from the source face of the lattice        
        if (node->id >= 1 && node->id <=100){
            injectable_nodes.push_back( node );
        }
        
        //List of collection nodes, from the drain face of the lattice
        if (node->id >=1901 && node->id <=2000){
            collection_nodes.push_back( node );
        }
        
        //node->PrintNode();   
    }
    
    delete stmt;
    
    //List of neighbours (from neighbour list for charge transfer - state.sql "pairs" table) 
    //List of rates for electron transfer from seg 1 to seg 2 and vice versa
    stmt = db.Prepare("SELECT seg1, seg2, drX, drY, drZ, rate12e, rate21e FROM pairs;");
    while (stmt->Step() != SQLITE_DONE)     
    {           
        int seg1 = stmt->Column<int>(0);
        int seg2 = stmt->Column<int>(1);
        
        BNode* node1 = GetNode( seg1 );
        BNode* node2 = GetNode( seg2 );
        
        double dx_pbc = stmt->Column<double>(2);
        double dy_pbc = stmt->Column<double>(3);
        double dz_pbc = stmt->Column<double>(4);
        double rate12e = 10E11; // 1 -> 2
        double rate21e = 10E11; // 2 -> 1
       
        votca::tools::vec distance_pbc(dx_pbc, dy_pbc, dz_pbc);
        Edge* edge12 = new Edge(node1, node2, distance_pbc, rate12e); 
        Edge* edge21 = new Edge(node2, node1, -distance_pbc, rate21e);
        
        edges.push_back( edge12 );
        edges.push_back( edge21 );
        
        // make sure no duplicates are added
        node1->AddEdge( edge12 );
        node2->AddEdge( edge21 );
        
         
    }
    delete stmt;

}

    void TerminalGraph::Print(){
    
        for (std::vector< BNode* >::iterator node = nodes.begin() ; node != nodes.end(); ++node) {
            (*node)->PrintNode();
        }
    };
 
    
}}



#endif

