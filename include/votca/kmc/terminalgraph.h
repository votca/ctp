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

    typedef std::vector< BNode* >::iterator iterator;
    typedef const std::vector< BNode* >::iterator const_iterator;

    // iterator over the source (source) nodes
    iterator source_nodes_begin() { return source_nodes.begin(); }
    iterator source_nodes_end() { return source_nodes.end(); } 
    int source_nodes_size() {return source_nodes.size(); }
    
    // iterator over the lattice nodes
    iterator lattice_nodes_begin() { return lattice_nodes.begin(); }
    iterator lattice_nodes_end() { return lattice_nodes.end(); } 
    int lattice_nodes_size() {return lattice_nodes.size(); }
    
    //iterator over the drain (drain) nodes
    iterator drain_nodes_begin() { return drain_nodes.begin(); }
    iterator drain_nodes_end() { return drain_nodes.end(); } 
    int drain_nodes_size() {return drain_nodes.size(); }
    
    // iterator over the injectable nodes/where a carrier can be injected (nodes at the source face)
    iterator injectable_nodes_begin() { return injectable_nodes.begin(); }
    iterator injectable_nodes_end() { return injectable_nodes.end(); } 
    int injectable_nodes_size() {return injectable_nodes.size();}
    
    // iterator over the collectable nodes/where a carrier can be collected ( nodes at the drain face)
    iterator collectable_nodes_begin() { return collectable_nodes.begin(); }
    iterator collectable_nodes_end() { return collectable_nodes.end(); } 
    int collectable_nodes_size() {return collectable_nodes.size();}
    
    
    void Create_source_electrode(int ncarriers, double source_electrode_x, double source_electrode_y, double source_electrode_z);
    void Create_drain_electrode(int ncarriers, double drain_electrode_x, double drain_electrode_y, double drain_electrode_z);
    
    void Load_Graph(std::string filename, double inject_x, double inject_y, double inject_z, double collect_x, double collect_y, double collect_z);    
    void Load_Neighbours(std::string filename);
    void Print();
    
    // add a node to the vector of lattice nodes
    BNode* AddLatticeNode() {
        BNode *node = new BNode(); 
        nodes.push_back( node );
        lattice_nodes.push_back( node );
        return node;
    };
  
    // add a node to the vector of source nodes
    BNode* AddSourceNode() {
        BNode *node = new BNode();
        nodes.push_back( node );
        source_nodes.push_back( node );
        return node;
    };
    
    // add a node to the vector of drain nodes
    BNode* AddDrainNode() {
        BNode *node = new BNode();
        nodes.push_back( node );
        drain_nodes.push_back( node );
        return node;
    };
    
    //selecting a node from the lattice nodes
    BNode* GetLatticeNode( int id ) {
        std::vector< BNode* >::iterator node = lattice_nodes.begin() ;
        while ( (*node)->id != id  ) node++ ;
        return *node;
    };
    
    // node selection from the source nodes
    BNode* GetSourceNode( int id ) {
        std::vector< BNode* >::iterator node = source_nodes.begin() ;
        while ( (*node)->id != id ) node++ ;
        return *node;
    };
    
    //selecting a node from the drain nodes
    BNode* GetDrainNode( int id ) {
        std::vector< BNode* >::iterator node = drain_nodes.begin() ;
        while ( (*node)->id != id ) node++ ;
        return *node;
    };
  
private:

    std::vector< BNode* > nodes;
    std::vector< BNode* > lattice_nodes;
    std::vector< BNode* > source_nodes;    
    std::vector< BNode* > drain_nodes;
    std::vector < BNode* > injectable_nodes;
    std::vector < BNode* > collectable_nodes;
    std::vector< Edge* > edges;
    
};

void TerminalGraph::Create_source_electrode (int ncarriers, double source_electrode_x, double source_electrode_y, double source_electrode_z){
    
    //Add the injection node - Source (in front of the face of injectable nodes)     
    for ( int carrier = 1; carrier <= ncarriers; ++carrier ) {  
        BNode *source_node = AddSourceNode();
        source_node->id = carrier;
        double x_source = source_electrode_x;
        double y_source = source_electrode_y;
        double z_source = source_electrode_z;  
        myvec source_position = myvec(x_source, y_source, z_source); 
        source_node->position = source_position;
  
    }
}  

void TerminalGraph::Load_Graph(std::string filename, double inject_x, double inject_y, double inject_z, double collect_x, double collect_y, double collect_z) {
      
    std::cout << "Loading the graph from " << filename << std::endl;
    
    // initialising the database file
    votca::tools::Database db;
    db.Open( filename );
        
    votca::tools::Statement *stmt = db.Prepare("SELECT id-1, posX, posY, posZ, UnCnNe, UnCnNh, UcNcCe, UcNcCh, eAnion, eNeutral, eCation, UcCnNe, UcCnNh FROM segments;");

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
        
        //Energies for each node - for electron and hole
        node->reorg_intorig_electron =  stmt->Column<double>(4);
        node->reorg_intorig_hole =  stmt->Column<double>(5);
        
        node->reorg_intdest_electron =  stmt->Column<double>(6);
        node->reorg_intdest_hole =  stmt->Column<double>(7);
        
        node->eAnion = stmt->Column<double>(8);
        node->eNeutral = stmt->Column<double>(9);
        node->eCation = stmt->Column<double>(10);
        
        node->internal_energy_electron = stmt->Column<double>(11);
        node->internal_energy_hole = stmt->Column<double>(12);
        
        node->site_energy_electron = node->eAnion + node->internal_energy_electron;
        node->site_energy_hole = node->eCation + node->internal_energy_hole;
        
        //Only add an injectable node from the source facing side of the lattice
        //defined as a cut-off for example: x-cut-off at x=3.0 all values below 3.0 are injectable nodes
        if(inject_x != 0 && inject_y==0 && inject_z==0) {
            if ( x <= inject_x ){
                injectable_nodes.push_back( node );
            }
        }
        else if(inject_y != 0 && inject_x==0 && inject_z==0) {
            if ( y <= inject_y ){
                injectable_nodes.push_back( node );
            }
        }        
        else if(inject_z != 0 && inject_x==0 && inject_y==0) {
            if ( z <= inject_z ){
                injectable_nodes.push_back( node );
            }
        }

        //List of collection nodes, from the drain facing side of the lattice
        //defined as a cut-off for example: x-cut-off at x=10.0 all values above 10.0 are collectable nodes         
        if(collect_x != 0 && collect_y==0 && collect_z==0) {
            if ( x >= collect_x ){
                collectable_nodes.push_back( node );
            }
        }
        else if(collect_y != 0 && collect_x==0 && collect_z==0) {
            if ( y >= collect_y ){
                collectable_nodes.push_back( node );
            }
        }       
        else if(collect_z != 0 && collect_x==0 && collect_y==0) {
            if ( z >= collect_z ){
                collectable_nodes.push_back( node );
            }
        }    
        //node->PrintNode();   
    }
  
    delete stmt;
}

void TerminalGraph::Create_drain_electrode (int ncarriers, double drain_electrode_x, double drain_electrode_y, double drain_electrode_z){
    
    //Add the collection nodes - Drain (behind the face of collectable nodes)
    //has to be done after the graph is loaded - drain id should continue after final graph node id
    for ( int carrier = 1; carrier <= ncarriers; ++carrier ) {
        BNode *drain_node = AddDrainNode();
        drain_node->id = lattice_nodes_size()+carrier ;
        double x_drain = drain_electrode_x;
        double y_drain = drain_electrode_y;
        double z_drain = drain_electrode_z; 
        myvec drain_position = myvec(x_drain, y_drain, z_drain);
        drain_node->position = drain_position; 
    }
       
}

void TerminalGraph::Load_Neighbours(std::string filename) {
    
    for (std::vector< BNode* >::iterator source_node = source_nodes_begin() ; source_node != source_nodes_end(); ++source_node){

        BNode* source = GetSourceNode( (*source_node)->id );
    
        for (std::vector< BNode* >::iterator injection_edge = injectable_nodes.begin() ; injection_edge != injectable_nodes.end(); ++injection_edge){

            double dx_inj = 0.0;
            double dy_inj = 0.0;
            double dz_inj = 0.0;
            
            //double E_Fermi_inj = 0.0; //Fermi level of the source electrode
            
            // rate: source_node -> injection face 
            double rate_inject_hole = 10E17;
            double rate_inject_electron = 10E17;
            
            votca::tools::vec distance(dx_inj, dy_inj, dz_inj);
                
            Edge* edge_injection = new Edge(source, (*injection_edge), distance, rate_inject_electron, rate_inject_hole);
            edges.push_back( edge_injection );
            source->AddEdge( edge_injection );
 
        }
    }
    //Adding the drain node (drain) as an edge of each of the collectable nodes 
    for (std::vector< BNode* >::iterator drain_node = drain_nodes_begin() ; drain_node != drain_nodes_end(); ++drain_node){
        BNode* drain = GetDrainNode( (*drain_node)->id );

        //std::cout << "drain node " << (*drain_node)->id << std::endl;
        //Each collectable node has the drain node as an edge
        for (std::vector< BNode* >::iterator collection_edge = collectable_nodes.begin() ; collection_edge != collectable_nodes.end(); ++collection_edge){

            double dx_col = 0.0;
            double dy_col = 0.0;
            double dz_col = 0.0;
            
            //double E_Fermi_col = -1.0; //Fermi level of the drain electrode
            
            // rate : collection face -> drain node 
            double rate_collect_hole = 10E17;
            double rate_collect_electron = 10E17;
            
            votca::tools::vec distance(dx_col, dy_col, dz_col);
 
            Edge* edge_collection = new Edge( (*collection_edge), drain, distance, rate_collect_electron, rate_collect_hole);
            edges.push_back( edge_collection );
            (*collection_edge)->AddEdge( edge_collection );
          
        }
        
        for (std::vector< BNode* >::iterator source_node = source_nodes_begin() ; source_node != source_nodes_end(); ++source_node){            
            
            BNode* source = (*source_node);
            // The drain node has the source node as an edge - carriers can be returned to the injection reservoir
            double dx_col_inj = 0.0;
            double dy_col_inj = 0.0;
            double dz_col_inj = 0.0;
            
            double rate_return_hole = 10E20; // rate : drain node -> source node (outer circuit motion considered instantaneous)
            double rate_return_electron = 10E20;
            
            votca::tools::vec distance(dx_col_inj, dy_col_inj, dz_col_inj);
    
            Edge* edge_col_inj = new Edge( drain, source, distance, rate_return_electron, rate_return_hole);
            edges.push_back(edge_col_inj);
            drain->AddEdge(edge_col_inj);
            
        }
    }
 
        
    // initialising the database file
    votca::tools::Database db;
    db.Open( filename );
    
    //List of neighbours (from neighbour list for charge transfer - state.sql "pairs" table) 
    //List of rates for hole transfer from seg 1 to seg 2 and vice versa
    votca::tools::Statement *stmt = db.Prepare("SELECT seg1, seg2, drX, drY, drZ, rate12e, rate21e, rate12h, rate21h, Jeff2e, Jeff2h FROM pairs;");
    
    while (stmt->Step() != SQLITE_DONE)     
    {         
        int seg1 = stmt->Column<int>(0);
        int seg2 = stmt->Column<int>(1);
       
        BNode* node1 = GetLatticeNode( seg1 );
        BNode* node2 = GetLatticeNode( seg2 );
        
        double dx_pbc = stmt->Column<double>(2);
        double dy_pbc = stmt->Column<double>(3);
        double dz_pbc = stmt->Column<double>(4);
        
        double rate12e = stmt->Column<double>(5); // 1 -> 2
        double rate21e = stmt->Column<double>(6); // 2 -> 1
        
        double rate12h = stmt->Column<double>(7); // 1 -> 2
        double rate21h = stmt->Column<double>(8); // 2 -> 1
   
        double Jeff2e = stmt->Column<double>(9);
        double Jeff2h = stmt->Column<double>(10);
        
        votca::tools::vec distance_pbc(dx_pbc, dy_pbc, dz_pbc);
 
        Edge* edge12 = new Edge(node1, node2, distance_pbc, rate12e, rate12h);
        Edge* edge21 = new Edge(node2, node1, -distance_pbc, rate21e, rate21h);
 
        node1->AddEdge(edge12);
        node2->AddEdge( edge21 );

        edges.push_back( edge12 );
        edges.push_back( edge21 );
                          
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

