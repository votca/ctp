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
    
    // iterator over the nodes
    iterator nodes_begin() { return nodes.begin(); }
    iterator nodes_end() { return nodes.end(); } 
    int nodes_size() {return nodes.size(); }
       
    //iterator over the drain (drain) nodes
    iterator drain_nodes_begin() { return drain_nodes.begin(); }
    iterator drain_nodes_end() { return drain_nodes.end(); } 
    int drain_nodes_size() {return drain_nodes.size(); }
    
    // iterator over the injectable nodes/where a carrier can be injected (nodes at the source face)
    //iterator injectable_nodes_begin() { return injectable_nodes.begin(); }
    //iterator injectable_nodes_end() { return injectable_nodes.end(); } 
    //int injectable_nodes_size() {return injectable_nodes.size();}
    
    // iterator over the collectable nodes/where a carrier can be collected ( nodes at the drain face)
    //iterator collectable_nodes_begin() { return collectable_nodes.begin(); }
    //iterator collectable_nodes_end() { return collectable_nodes.end(); } 
    //int collectable_nodes_size() {return collectable_nodes.size();}
    
    
    //void Create_source_electrode(int ncarriers, double source_electrode_x, double source_electrode_y, double source_electrode_z);
    //void Create_drain_electrode(int ncarriers, double drain_electrode_x, double drain_electrode_y, double drain_electrode_z);
    
    void Load_Graph(std::string filename, double temperature);
    //void Load_injectable_collectable(std::string field_direction);
    //void Load_Electrode_Neighbours(std::string filename);
    void Load_Rates(std::string filename);
    
    void Rates_Calculation(std::string filename, int nelectrons, int nholes, double fieldX, double fieldY, double fieldZ, double temperature);
    
    void Print();
    
    // add a node to the vector of nodes
    BNode* AddNode() {
        BNode *node = new BNode(); 
        nodes.push_back( node );
        return node;
    };
      
    // add a node to the vector of source nodes
    BNode* AddSourceNode() {
        BNode *node = new BNode();
        source_nodes.push_back( node );
        return node;
    };
   
    // add a node to the vector of drain nodes
    BNode* AddDrainNode() {
        BNode *node = new BNode();
        drain_nodes.push_back( node );
        return node;
    };
    
    //selecting a node from the nodes
    BNode* GetNode( int id ) {
        std::vector< BNode* >::iterator node = nodes.begin() ;
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
    std::vector< BNode* > source_nodes;    
    std::vector< BNode* > drain_nodes;
    //std::vector < BNode* > injectable_nodes;
    //std::vector < BNode* > collectable_nodes;
    std::vector< Edge* > edges;
};

/*void TerminalGraph::Create_source_electrode (int ncarriers, double source_electrode_x, double source_electrode_y, double source_electrode_z){
    
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
*/

//void TerminalGraph::Load_Graph(std::string filename, double inject_x, double inject_y, double inject_z, double collect_x, double collect_y, double collect_z) {
void TerminalGraph::Load_Graph(std::string filename, double temperature) {   
 
    double kB   = 8.617332478E-5; // eV/K
    double sigma = 0.1; // energetic disorder ----> should be read in
    
    std::cout << "Loading the graph from " << filename << std::endl;
    
    // initialising the database file
    votca::tools::Database db;
    db.Open( filename );
        
    votca::tools::Statement *stmt = db.Prepare("SELECT name, id-1, posX, posY, posZ, UnCnNe, UnCnNh, UcNcCe, UcNcCh, eAnion, eNeutral, eCation, UcCnNe, UcCnNh FROM segments;");
  
    while (stmt->Step() != SQLITE_DONE)
    {             
        string name = stmt->Column<string>(0);
        
        //if (name == "SSS"){
            //BNode *source_node = AddSourceNode();
            //int id = stmt->Column<int>(1);
            //source_node->id = id+1;             
        //}
        //if (name == "DDD"){
            //BNode *drain_node = AddDrainNode();
            //int id = stmt->Column<int>(1);
            //drain_node->id = id+1;
        //}

        BNode *node = AddNode();
        int id = stmt->Column<int>(1);
        node->id = id+1;
           
        // position in nm
        double x = stmt->Column<double>(2);
        double y = stmt->Column<double>(3);
        double z = stmt->Column<double>(4); 
        
        myvec position = myvec(x, y, z); 
        node->position = position;
        
        //Energies for each node - for electron and hole
        node->reorg_intorig_electron =  stmt->Column<double>(5);
        node->reorg_intorig_hole =  stmt->Column<double>(6);
        
        node->reorg_intdest_electron =  stmt->Column<double>(7);
        node->reorg_intdest_hole =  stmt->Column<double>(8);
        
        node->eAnion = stmt->Column<double>(9);
        node->eNeutral = stmt->Column<double>(10);
        node->eCation = stmt->Column<double>(11);
        
        node->internal_energy_electron = stmt->Column<double>(12);
        node->internal_energy_hole = stmt->Column<double>(13);
        
        node->site_energy_electron = node->eAnion + node->internal_energy_electron;
        node->site_energy_hole = node->eCation + node->internal_energy_hole;
        
        node->hi_e = node->eAnion * (-(sigma*sigma)/(2*kB*temperature));
        node->hi_h = node->eCation * (-(sigma*sigma)/(2*kB*temperature));
        
        //node->PrintNode();   

    }
    
    delete stmt;
}
/*void TerminalGraph::Load_injectable_collectable(std::string field_direction){
    
    //Start with the first node - use it's position as the initial minimum position
    BNode* node1 = GetLatticeNode( 1 );
    
    double x_pos1 = node1->position.getX();
    double y_pos1 = node1->position.getY();
    double z_pos1 = node1->position.getZ(); 
        
    double minX = x_pos1;
    double minY = y_pos1;
    double minZ = z_pos1;
    
    //Use the last node to make an initial maximum position
    int max_node = lattice_nodes_size();
    BNode* node2 = GetLatticeNode( max_node );
    
    double x_pos2 = node2->position.getX();
    double y_pos2 = node2->position.getY();
    double z_pos2 = node2->position.getZ();
    
    double maxX = x_pos2;
    double maxY = y_pos2;
    double maxZ = z_pos2; 
        
    //Then loop through all nodes to check the minimum and maximum position
    for (std::vector< BNode* >::iterator node = lattice_nodes_begin() ; node != lattice_nodes_end(); ++node){
             
        BNode* node_pos = GetLatticeNode( (*node)->id );      
        double x_pos_node = node_pos->position.getX();
        double y_pos_node = node_pos->position.getY();
        double z_pos_node = node_pos->position.getZ(); 
     
        if(x_pos_node < minX) {minX = x_pos_node;}
        else if(y_pos_node < minY) {minY = y_pos_node;}
        else if(z_pos_node < minZ) {minZ = z_pos_node;}
        else if(x_pos_node > maxX) {maxX = x_pos_node;}
        else if(y_pos_node > maxY) {maxY = y_pos_node;}
        else if(z_pos_node > maxZ) {maxZ = z_pos_node;}
        
        //When the field is in the x direction, the face for injection will be the first x face
        //Only add these nodes to the injectable nodes
        if (field_direction == "X"){
            if ( x_pos_node == minX ){              
                injectable_nodes.push_back( node_pos );
            }
        }
        else if (field_direction == "Y"){
            if ( y_pos_node == minY ){
                injectable_nodes.push_back ( node_pos );
            }
        }
        else if (field_direction == "Z"){
            if ( z_pos_node == minZ ){
                injectable_nodes.push_back ( node_pos );
            }
        }
    
        //When the field is in the x direction, the face for collection will be the last x face
        //Only add these nodes to the collectable nodes
        if (field_direction == "X"){
            if ( x_pos_node == maxX ){              
                collectable_nodes.push_back( node_pos );
            }    
        }
        else if (field_direction == "Y"){
            if ( y_pos_node == maxY ){
                collectable_nodes.push_back ( node_pos );
            }
        }
        else if (field_direction == "Z"){
            if ( z_pos_node == maxZ ){
                collectable_nodes.push_back ( node_pos );
            }
        }
    }
    
}
*/

/*void TerminalGraph::Create_drain_electrode (int ncarriers, double drain_electrode_x, double drain_electrode_y, double drain_electrode_z){
    
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
*/

/*void TerminalGraph::Load_Electrode_Neighbours(std::string filename) {
    
    for (std::vector< BNode* >::iterator source_node = source_nodes_begin() ; source_node != source_nodes_end(); ++source_node){

        BNode* source = GetSourceNode( (*source_node)->id );
    
        for (std::vector< BNode* >::iterator injection_edge = injectable_nodes.begin() ; injection_edge != injectable_nodes.end(); ++injection_edge){
            
            BNode* injection = (*injection_edge);
            
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
 
}
*/

void TerminalGraph::Load_Rates(std::string filename) {
    
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
       
        BNode* node1 = GetNode( seg1 );
        BNode* node2 = GetNode( seg2 );
        
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
    
    for (std::vector< BNode* >::iterator drain_node = drain_nodes_begin() ; drain_node != drain_nodes_end(); ++drain_node){
        
        BNode* drain = GetDrainNode( (*drain_node)->id );
        
        BNode* node1 = GetNode(drain->id);
        
        if (drain->id == node1->id){
            
            for (std::vector< BNode* >::iterator source_node = source_nodes_begin() ; source_node != source_nodes_end(); ++source_node){            
            
                BNode* source = GetNode((*source_node)->id);
                
                // The drain node has the source node as an edge - carriers can be returned to the injection reservoir
                double dx_col_inj = 0.0;
                double dy_col_inj = 0.0;
                double dz_col_inj = 0.0;
            
                double rate_return_hole = 10E20; // rate : drain node -> source node (outer circuit motion considered instantaneous)
                double rate_return_electron = 10E20;
            
                votca::tools::vec distance(dx_col_inj, dy_col_inj, dz_col_inj);
    
                Edge* edge_col_inj = new Edge( node1, source, distance, rate_return_electron, rate_return_hole);
                node1->AddEdge(edge_col_inj);
                edges.push_back(edge_col_inj);           
            }       
        }
    }

}

void TerminalGraph::Rates_Calculation(std::string filename, int nelectrons, int nholes, double fieldX, double fieldY, double fieldZ, double temperature){
    
    double kB   = 8.617332478E-5; // eV/K
    double hbar = 6.5821192815E-16; // eV*s
    double Pi   = 3.14159265358979323846;
    double eps0 = 8.85418781762E-12/1.602176565E-19; // e**2/eV/m = 8.85418781762E-12 As/Vm
    double epsr = 3.0; // relative material permittivity
        
    double charge_e = -1.0;
    double charge_h = 1.0;

    //Calculation of the initial Marcus rates - allows the rates to be calculated with varying field and temperature    
    std::cout << "Calculating initial Marcus rates." << std::endl;
    std::cout << "Temperature T = " << temperature << " K." << std::endl;
    std::cout << "Field: (" << fieldX << ", " << fieldY << ", " << fieldZ << ") V/m" << std::endl;
    
    votca::tools::Database db;
    db.Open( filename );
    
    votca::tools::Statement *stmt = db.Prepare("SELECT seg1, seg2, drX, drY, drZ, Jeff2e, Jeff2h, lOe, lOh FROM pairs;");
    
    while (stmt->Step() != SQLITE_DONE)     
    {         
        int seg1 = stmt->Column<int>(0);
        int seg2 = stmt->Column<int>(1);
       
        BNode* node1 = GetNode( seg1 );
        BNode* node2 = GetNode( seg2 );
      
        //distance (nm) from node 1 to node 2) - keep in (nm) as all other objects use (nm)
        double dX = stmt->Column<double>(2);
        double dY = stmt->Column<double>(3);
        double dZ = stmt->Column<double>(4);
       
        votca::tools::vec distance(dX, dY, dZ);
        
        //distance (nm) from node 1 to node 2) converted to (m) for dG field calculation
        double _dX = dX*1E-9;
        double _dY = dY*1E-9;
        double _dZ = dZ*1E-9;
        
        //Effect of field on charge carrier (electron and hole)
        double dG_Field_e = charge_e * (_dX*fieldX +  _dY*fieldY + _dZ*fieldZ);
        double dG_Field_h = charge_h * (_dX*fieldX +  _dY*fieldY + _dZ*fieldZ);
   
        double Jeff2e = stmt->Column<double>(5);
        double Jeff2h = stmt->Column<double>(6);
        
        double reorg_out_e = stmt->Column<double>(7); 
        double reorg_out_h = stmt->Column<double>(8);

        //reorganisation energy difference for nodes 1 and 2 (for electrons and holes)
        //1->2
        double reorg_e12 = node1->reorg_intorig_electron + node2->reorg_intdest_electron + reorg_out_e;
        double reorg_h12 = node1->reorg_intorig_hole + node2->reorg_intdest_hole + reorg_out_h;
        //2->1
        double reorg_e21 = node2->reorg_intorig_electron + node1->reorg_intdest_electron + reorg_out_e;
        double reorg_h21 = node2->reorg_intorig_hole + node1->reorg_intdest_hole + reorg_out_h;
        
        //Site energy difference for nodes 1 and 2 (for electrons and holes) - Internal
        //1->2
        double dG_Site_e12 = node2->site_energy_electron - node1->site_energy_electron;
        double dG_Site_h12 = node2->site_energy_hole - node1->site_energy_hole;
        //2->1
        double dG_Site_e21 = node1->site_energy_electron - node2->site_energy_electron;
        double dG_Site_h21 = node1->site_energy_hole - node2->site_energy_hole;
       
        //Site energy difference and field difference for charge carrier hop (internal + external)
        //1->2
        double dG_e12 = dG_Site_e12 - dG_Field_e;
        double dG_h12 = dG_Site_h12 - dG_Field_h;
        //2->1
        double dG_e21 = dG_Site_e21 - dG_Field_e;
        double dG_h21 = dG_Site_h21 - dG_Field_h;
        
        //KMC calculated Marcus rates
        //1->2
        double cal_rate12e = 2*Pi/hbar * Jeff2e/sqrt(4*Pi*reorg_e12*kB*temperature) * exp(-(dG_e12+reorg_e12)*(dG_e12+reorg_e12) / (4*reorg_e12*kB*temperature));
        double cal_rate12h = 2*Pi/hbar * Jeff2h/sqrt(4*Pi*reorg_h12*kB*temperature) * exp(-(dG_h12+reorg_h12)*(dG_h12+reorg_h12) / (4*reorg_h12*kB*temperature));
        //2->1
        double cal_rate21e = 2*Pi/hbar * Jeff2e/sqrt(4*Pi*reorg_e21*kB*temperature) * exp(-(dG_e21+reorg_e21)*(dG_e21+reorg_e21) / (4*reorg_e21*kB*temperature));
        double cal_rate21h = 2*Pi/hbar * Jeff2h/sqrt(4*Pi*reorg_h21*kB*temperature) * exp(-(dG_h21+reorg_h21)*(dG_h21+reorg_h21) / (4*reorg_h21*kB*temperature));
        
        //std::cout << "Rate 12h " << cal_rate12h << "   Rate 21h: " << cal_rate21h << std::endl;
        
        //add the new edge with calculated rate
        Edge* edge12 = new Edge(node1, node2, distance, cal_rate12e, cal_rate12h);
        Edge* edge21 = new Edge(node2, node1, -distance, cal_rate21e, cal_rate21h);
 
        node1->AddEdge(edge12);
        node2->AddEdge( edge21 );

        edges.push_back( edge12 );
        edges.push_back( edge21 );      

    }
       
    delete stmt;
    
    //closed circuit - for the return of carriers from the drain to the source 
    for (std::vector< BNode* >::iterator drain_node = drain_nodes_begin() ; drain_node != drain_nodes_end(); ++drain_node){
        
        BNode* drain = GetDrainNode( (*drain_node)->id );
        
        BNode* node1 = GetNode(drain->id);
        
        if (drain->id == node1->id){
            
            for (std::vector< BNode* >::iterator source_node = source_nodes_begin() ; source_node != source_nodes_end(); ++source_node){            
            
                BNode* source = GetNode((*source_node)->id);
                
                // The drain node has the source node as an edge - carriers can be returned to the injection reservoir
                double dx_col_inj = 0.0;
                double dy_col_inj = 0.0;
                double dz_col_inj = 0.0;
            
                double rate_return_hole = 10E20; // rate : drain node -> source node (outer circuit motion considered instantaneous)
                double rate_return_electron = 10E20;
            
                votca::tools::vec distance(dx_col_inj, dy_col_inj, dz_col_inj);
    
                Edge* edge_col_inj = new Edge( node1, source, distance, rate_return_electron, rate_return_hole);
                node1->AddEdge(edge_col_inj);
                edges.push_back(edge_col_inj);           
            }       
        }
    }
}

void TerminalGraph::Print(){
    
    for (std::vector< BNode* >::iterator node = nodes.begin() ; node != nodes.end(); ++node) {
            (*node)->PrintNode();
    }
};
 
    
}}



#endif

