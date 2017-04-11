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

#include "state.h"

namespace votca { namespace kmc {
  
class TerminalGraph {
public:
    
    // iterator over carriers
    typedef std::vector< BNode* >::iterator iterator;
    typedef const std::vector< BNode* >::iterator const_iterator;
    
    iterator nodes_begin() { return nodes.begin(); }
    iterator nodes_end() { return nodes.end(); } 
    int nodes_size() {return nodes.size(); }

    iterator inject_nodes_begin() { return inject_nodes.begin(); }
    iterator inject_nodes_end() { return inject_nodes.end(); } 
    int inject_nodes_size() {return inject_nodes.size(); }
    
    iterator lattice_nodes_begin() { return lattice_nodes.begin(); }
    iterator lattice_nodes_end() { return lattice_nodes.end(); } 
    int lattice_nodes_size() {return lattice_nodes.size(); }
    
    iterator collect_nodes_begin() { return collect_nodes.begin(); }
    iterator collect_nodes_end() { return collect_nodes.end(); } 
    int collect_nodes_size() {return collect_nodes.size(); }
    
    iterator injectable_nodes_begin() { return injectable_nodes.begin(); }
    iterator injectable_nodes_end() { return injectable_nodes.end(); } 
    int injectable_nodes_size() {return injectable_nodes.size();}
    
    iterator collectable_nodes_begin() { return collectable_nodes.begin(); }
    iterator collectable_nodes_end() { return collectable_nodes.end(); } 
    int collectable_nodes_size() {return collectable_nodes.size();}
    
    void Create_electrodes(int nelectrons);
    void Load(std::string filename);    
    void Print();
    
    // add a node to the vector of Nodes
    BNode* AddLatticeNode() {
        BNode *node = new BNode(); 
        nodes.push_back( node );
        lattice_nodes.push_back( node );
        return node;
    };
  
    BNode* AddInjectNode() {
        BNode *node = new BNode();
        nodes.push_back( node );
        inject_nodes.push_back( node );
        return node;
    };
    
    BNode* AddCollectNode() {
        BNode *node = new BNode();
        nodes.push_back( node );
        collect_nodes.push_back( node );
        return node;
    };
    
    BNode* GetLatticeNode( int id ) {
        std::vector< BNode* >::iterator node = lattice_nodes.begin() ;
        while ( (*node)->id != id  ) node++ ;
        return *node;
    };
    
    BNode* GetNode( int id ) {
        std::vector< BNode* >::iterator node = nodes.begin() ;
        while ( (*node)->id != id  ) node++ ;
        return *node;
    };
    
    // node selection from the injectable nodes
    BNode* GetInjectNode( int id ) {
        std::vector< BNode* >::iterator node = inject_nodes.begin() ;
        while ( (*node)->id != id ) node++ ;
        return *node;
    };
    
    BNode* GetCollectNode( int id ) {
        std::vector< BNode* >::iterator node = collect_nodes.begin() ;
        while ( (*node)->id != id ) node++ ;
        return *node;
    };
  
private:

    std::vector< BNode* > nodes;
    std::vector< BNode* > lattice_nodes;
    std::vector< BNode* > inject_nodes;    
    std::vector< BNode* > collect_nodes;
    std::vector < BNode* > injectable_nodes;
    std::vector < BNode* > collectable_nodes;
    std::vector< Edge* > edges;
    
};

void TerminalGraph::Create_electrodes (int nelectrons){
    
    //Add the injection node - Source (in front of the face of injectable nodes)  
    
    for ( int electron = 1; electron <= nelectrons; ++electron ) {  
        BNode *inject_node = AddInjectNode();
        inject_node->id = electron;
        double x_inject = 0.0;
        double y_inject = 5.0;
        double z_inject = 5.0;  
        myvec inject_position = myvec(x_inject, y_inject, z_inject); 
        inject_node->position = inject_position;
  
    }
    
    //Add the collection nodes - Drain (behind the face of collectable nodes)
    for ( int electron = 1; electron <= nelectrons; ++electron ) {
        BNode *collect_node = AddCollectNode();
        collect_node->id = 2000+electron ;
        double x_collect = 20.0;
        double y_collect = 5.0;
        double z_collect = 5.0; 
        myvec collect_position = myvec(x_collect, y_collect, z_collect);
        collect_node->position = collect_position;   
    }
       
}

void TerminalGraph::Load(std::string filename) {
    
    State* state;
    std::cout << "Loading the graph from " << filename << std::endl;
    
    // initialising the database file
    votca::tools::Database db;
    db.Open( filename );
        
    votca::tools::Statement *stmt = db.Prepare("SELECT id-1, posX, posY, posZ FROM segments;");

    while (stmt->Step() != SQLITE_DONE)
    {
        BNode *node = AddLatticeNode();
        int id = stmt->Column<int>(0);
        node->id = id+1;

        // position in nm
        double x = stmt->Column<double>(1);
        double y = stmt->Column<double>(2);
        double z = stmt->Column<double>(3);       
   
        myvec position = myvec(x, y, z); 
        node->position = position;
        
        //Only add an injectable node from the source facing side of the lattice        
        //if (node->id >= 1 && node->id <=100){
        double injection_face = 0.5;
        if ( x == injection_face ){
            injectable_nodes.push_back( node );
        }
        
        //List of collection nodes, from the drain facing side of the lattice
        double collection_face = 19.5;
        //if (node->id >=1901 && node->id <=2000){
        if ( x == collection_face ){
            collectable_nodes.push_back( node );
        }
        
        //node->PrintNode();   
    }
  
    delete stmt;
    

    for (std::vector< BNode* >::iterator inject_node = inject_nodes_begin() ; inject_node != inject_nodes_end(); ++inject_node){

        BNode* injecting_node = GetInjectNode( (*inject_node)->id );
    
        for (std::vector< BNode* >::iterator injection_edge = injectable_nodes.begin() ; injection_edge != injectable_nodes.end(); ++injection_edge){

            double dx_inj = 0.0;
            double dy_inj = 0.0;
            double dz_inj = 0.0;
            double rate_inj_e = 10E5; // rate: injecting_node -> injection face
                
            votca::tools::vec distance(dx_inj, dy_inj, dz_inj);
                
            Edge* edge_injection = new Edge(injecting_node, (*injection_edge), distance, rate_inj_e);
            edges.push_back( edge_injection );
            injecting_node->AddEdge( edge_injection );
 
        }
    }
    //Adding the collecting node (drain) as an edge of each of the collectable nodes 
    for (std::vector< BNode* >::iterator collect_node = collect_nodes_begin() ; collect_node != collect_nodes_end(); ++collect_node){
        BNode* collecting_node = GetCollectNode( (*collect_node)->id );

        //std::cout << "collect node " << (*collect_node)->id << std::endl;
        //Each collectable node has the collect node as an edge
        for (std::vector< BNode* >::iterator collection_edge = collectable_nodes.begin() ; collection_edge != collectable_nodes.end(); ++collection_edge){

            double dx_col = 0.0;
            double dy_col = 0.0;
            double dz_col = 0.0;
            double rate_col_e = 10E11; // rate : collection face -> collecting node
                
            votca::tools::vec distance(dx_col, dy_col, dz_col);
 
            Edge* edge_collection = new Edge( (*collection_edge), collecting_node, distance, rate_col_e);
            edges.push_back( edge_collection );
            (*collection_edge)->AddEdge( edge_collection );
          
        }
        
        for (std::vector< BNode* >::iterator inject_node = inject_nodes_begin() ; inject_node != inject_nodes_end(); ++inject_node){            
            
            BNode* injecting_node = (*inject_node);
            // The collecting node has the injecting node as an edge - carriers can be returned to the injection reservoir
            double dx_col_inj = 0.0;
            double dy_col_inj = 0.0;
            double dz_col_inj = 0.0;
            double rate_col_inj_e = 10E5; // rate : collecting node -> injecting node
                
            votca::tools::vec distance(dx_col_inj, dy_col_inj, dz_col_inj);
    
            Edge* edge_col_inj = new Edge( collecting_node, injecting_node, distance, rate_col_inj_e);
            edges.push_back(edge_col_inj);
            collecting_node->AddEdge(edge_col_inj);
            
        }
    }
 
    //List of neighbours (from neighbour list for charge transfer - state.sql "pairs" table) 
    //List of rates for electron transfer from seg 1 to seg 2 and vice versa
    stmt = db.Prepare("SELECT seg1, seg2, drX, drY, drZ, rate12e, rate21e FROM pairs;");
    while (stmt->Step() != SQLITE_DONE)     
    {         
        int seg1 = stmt->Column<int>(0);
        int seg2 = stmt->Column<int>(1);
       
        BNode* node1 = GetLatticeNode( seg1 );
        BNode* node2 = GetLatticeNode( seg2 );
        
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

