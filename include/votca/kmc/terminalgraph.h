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

//Terminal Graph can create injection and collection of carriers, via a "source" and a "drain" set of nodes

#ifndef __VOTCA_KMC_TERMINALGRAPH_H_
#define __VOTCA_KMC_TERMINALGRAPH_H_

#include <votca/kmc/bnode.h>

namespace votca { namespace kmc {
   
class TerminalGraph {
public:

    // iterator over carriers
    typedef std::vector< BNode* >::iterator iterator;
    typedef const std::vector< BNode* >::iterator const_iterator;
  
    
    void Load_Graph(std::string filename, double temperature);
    void Load_Rates(std::string filename);
    void Rates_Calculation(std::string filename, int nelectrons, int nholes, double fieldX, double fieldY, double fieldZ, double temperature);
    
    void Create_excited_inject_collect_nodes(int nexciteddonor);
    void Load_Excited_Graph(std::string filename);//for energy transfer - calculator:excited 
    void Load_Excited_pairs(std::string filename);
    void Excited_energy_transfer_with_inject_collect(std::string filename);//for energy transfer - calculator:excited 

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

        // add a node to the vector of Nodes - inject
    BNode* AddInjectNode() {
        BNode *node = new BNode();
        inject_nodes.push_back( node );
        //nodes.push_back( node );
        return node;
    };
    
    // add a node to the vector of Nodes - collect
    BNode* AddCollectNode() {
        BNode *node = new BNode();
        collect_nodes.push_back( node );
        //nodes.push_back( node );
        return node;
    };
 
    // node selection from the inject nodes
    BNode* GetInjectNode( int id ) {
        std::vector< BNode* >::iterator node = inject_nodes.begin() ;
        while ( (*node)->id != id ) node++ ;
        return *node;
    };
    
    // node selection from the collect nodes
    BNode* GetCollectNode( int id ) {
        std::vector< BNode* >::iterator node = collect_nodes.begin() ;
        while ( (*node)->id != id ) node++ ;
        return *node;
    };
    
    iterator nodes_begin() { return nodes.begin(); }
    iterator nodes_end() { return nodes.end(); } 
    int nodes_size() {return nodes.size(); }
    
    //iterator over the inject nodes/where a carrier can be injected
    iterator inject_nodes_begin() { return inject_nodes.begin(); }
    iterator inject_nodes_end() { return inject_nodes.end(); } 
    
    //iterator over the collectable nodes/where a carrier can be collected (collect nodes) 
    iterator collect_nodes_begin() { return collect_nodes.begin(); }
    iterator collect_nodes_end() { return collect_nodes.end(); } 
    
    int donor_nodes_size() {return donor_nodes.size();}
    int acceptor_nodes_size() {return acceptor_nodes.size();}
    int inject_nodes_size() {return inject_nodes.size();}
    int collect_nodes_size() {return collect_nodes.size();}
    
private:

    std::vector< BNode* > nodes;
    std::vector< Edge* > edges;
    std::vector < BNode* > donor_nodes;
    std::vector < BNode* > acceptor_nodes;
    std::vector <BNode*> inject_nodes;
    std::vector < BNode* > collect_nodes;
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
   
        //double Jeff2e = stmt->Column<double>(9);
        //double Jeff2h = stmt->Column<double>(10); 
        
        votca::tools::vec distance_pbc(dx_pbc, dy_pbc, dz_pbc);
 
        Edge* edge12 = new Edge(node1, node2, distance_pbc, rate12e, rate12h);
        Edge* edge21 = new Edge(node2, node1, -distance_pbc, rate21e, rate21h);
 
        node1->AddEdge(edge12);
        node2->AddEdge( edge21 );

        edges.push_back( edge12 );
        edges.push_back( edge21 );
 
    }
    delete stmt;
    
    for (std::vector< BNode* >::iterator drain_node = collect_nodes_begin() ; drain_node != collect_nodes_end(); ++drain_node){
        
        BNode* drain = GetCollectNode( (*drain_node)->id );
        
        BNode* node1 = GetNode(drain->id);
        
        if (drain->id == node1->id){
            
            for (std::vector< BNode* >::iterator source_node = inject_nodes_begin() ; source_node != inject_nodes_end(); ++source_node){            
            
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
    //double eps0 = 8.85418781762E-12/1.602176565E-19; // e**2/eV/m = 8.85418781762E-12 As/Vm
    //double epsr = 3.0; // relative material permittivity
        
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
    for (std::vector< BNode* >::iterator drain_node = collect_nodes_begin() ; drain_node != collect_nodes_end(); ++drain_node){
        
        BNode* drain = GetCollectNode( (*drain_node)->id );
        
        BNode* node1 = GetNode(drain->id);
        
        if (drain->id == node1->id){
            
            for (std::vector< BNode* >::iterator source_node = inject_nodes_begin() ; source_node != inject_nodes_end(); ++source_node){            
            
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

void TerminalGraph::Load_Excited_Graph(std::string filename) {
    
    std::cout << "Loading the graph from " << filename << std::endl;
    
    // initialising the database file
    votca::tools::Database db;
    db.Open( filename );

    votca::tools::Statement *stmt = db.Prepare("SELECT id, type, posX, posY, posZ FROM segments;");
    //std::cout << "TEST" << std::endl;

    while (stmt->Step() != SQLITE_DONE)
    {
        BNode *node = AddNode();
        int id = stmt->Column<int>(0);
        node->id = id;
        
        //distinguish between acceptor/donor/matrix molecules for each node
        //type 1 = acceptor, 2 = matrix, 3 = donor
        int type = stmt->Column<int>(1);
        node->type = type;
        
        if (type == 3 ){
            donor_nodes.push_back( node );
        }
        else if ( type == 1 ){
            acceptor_nodes.push_back( node );
        }
        
        // position in nm
        double x = stmt->Column<double>(2);
        double y = stmt->Column<double>(3);
        double z = stmt->Column<double>(4);
        
        myvec position = myvec(x, y, z); 
        node->position = position; 
    }
    
    delete stmt; 
}

void TerminalGraph::Create_excited_inject_collect_nodes (int nexciteddonor){
    
    //Add the injection nodes    
    for ( int energy = 1; energy <= nexciteddonor; ++energy ) {  
        
        BNode *inject_node = AddInjectNode();
        inject_node->id = energy;
        
        double x_inject = 0.00;
        double y_inject = 0.00;
        double z_inject = 0.00; 

        myvec inject_position = myvec(x_inject, y_inject, z_inject); 
        inject_node->position = inject_position;

    }
    
    for ( int col_id = 1; col_id <= int(donor_nodes.size()); ++col_id){       
        
        BNode *collect_node = AddCollectNode();
        collect_node->id = col_id;
        
        double x_collect = 0.00;
        double y_collect = 0.00;
        double z_collect = 0.00;
                
        myvec collect_position = myvec(x_collect, y_collect, z_collect); 
        collect_node->position = collect_position;
    }
} 

void TerminalGraph::Load_Excited_pairs(std::string filename){
    
    votca::tools::Database db;
    db.Open( filename );
    
    double dexter_cut_off = 3E-9; //nm (3))
    double dexter_alpha = 2E-9; //nm factor to account for using centre of mass distance
    double R_FRET = 2.4E-9; //nm
    double fret_cut_off = 13.0E-9; //nm
    double kph = 1.6E6; // s-1 rate of phosphorescence 
    
    //List of neighbours (from neighbour list for charge transfer - state.sql "pairs" table) 
    //List of rates for electron transfer from seg 1 to seg 2 and vice versa
    
    votca::tools::Statement *stmt = db.Prepare("SELECT seg1, seg2, drX, drY, drZ FROM pairs;");
    while (stmt->Step() != SQLITE_DONE)     
    {           
        int seg1 = stmt->Column<int>(0);
        int seg2 = stmt->Column<int>(1);
        
        //node 1 = acceptor
        //node 2 = donor         
        BNode* node1 = GetNode( seg1 );
        BNode* node2 = GetNode( seg2 );
        
        //periodic distances - between centre of mass of each molecule
        double _dx_pbc = stmt->Column<double>(2);
        double _dy_pbc = stmt->Column<double>(3);
        double _dz_pbc = stmt->Column<double>(4);
       
        votca::tools::vec distance_pbc(_dx_pbc, _dy_pbc, _dz_pbc);
        
        double dx_pbc = _dx_pbc * 1E-9;
        double dy_pbc = _dy_pbc * 1E-9;
        double dz_pbc = _dz_pbc * 1E-9;
        
        double distance_AD = sqrt(((dx_pbc)*(dx_pbc))+((dy_pbc)*(dy_pbc))+((dz_pbc)*(dz_pbc)));
        //std::cout << distance_AD << std::endl;
        //Edge* edge12 = new Edge(node1, node2, distance_pbc, rate12e, rate12h); 
        //edges.push_back( edge12 );
        //node1->AddEdge( edge12 );

        // Energy transfer pairs and rates         
        if (distance_AD <= dexter_cut_off){
                    
            //double distance_AD_nm = distance_AD * 1e-9;
            double dexter_rate = 1E6 * exp((dexter_alpha/distance_AD)); //s-1
            //double dexter_rate = 1E6 * exp(-(distance_AD)); //s-1
                    
            double cal_rate12e = 0.00;
            double cal_rate12h = 0.00;
            double cal_dexter = dexter_rate;
            double cal_fret = 0.00;
            double cal_inject = 0.00;
            double cal_collect = 0.00;
            double cal_return = 0.00;
                    
            Edge* edge_dex = new Edge(node2, node1, distance_pbc, cal_rate12e, cal_rate12h, cal_dexter, cal_fret, cal_inject, cal_collect, cal_return); 
            node2->AddEdge(edge_dex);
            edges.push_back( edge_dex );
                    
            //std::cout << " Dexter rate added for " << node1->id << " & "  << node2->id << " at distance of " << distance_AD << " m " << std::endl;
            //std::cout << "Rate: " << dexter_rate << std::endl;
        }
                
        if (distance_AD > dexter_cut_off && distance_AD <= fret_cut_off) {
                    
            //double distance_AD_nm = distance_AD * 1E-9;
            //double FRET_rate = R_FRET/(pow(distance_AD,6));
            double fret = R_FRET/distance_AD;
            double FRET_rate = kph*(pow(fret,6)); //s-1
                    
            double cal_rate12e = 0.00;
            double cal_rate12h = 0.00;
            double cal_dexter = 0.00;
            double cal_fret = FRET_rate;
            double rate_inj = 0.00;
            double rate_col = 0.00;
            double rate_return = 0.00;
                    
            Edge* edge_fret = new Edge(node2, node1, distance_pbc, cal_rate12e, cal_rate12h, cal_dexter, cal_fret, rate_inj, rate_col, rate_return);
            node2->AddEdge(edge_fret);
            edges.push_back( edge_fret );
                    
            //std::cout << " FRET rate added for " << node1->id << " & "  << node2->id << " at distance of " << distance_AD << " m" << std::endl;
            //std::cout << "Rate: " << FRET_rate << std::endl;
        }
     
    }
    
    delete stmt;    
    
}

void TerminalGraph::Excited_energy_transfer_with_inject_collect(std::string filename){
    
    //std::cout << "Calculating excited energy transfer rates." << std::endl;

    //cut off distance below which dexter transfer would occur (above which, foster transfer would occur)
    //int dexter_cut_off = 1E-9; //m
    //double dexter_cut_off = 1; //nm
    //double R_FRET = 2.4; //nm

    // Inject energy pairs and rates        
    for (std::vector< BNode* >::iterator inject_node = inject_nodes_begin() ; inject_node != inject_nodes_end(); ++inject_node){

        BNode* inject = GetInjectNode( (*inject_node)->id );
        
        for (std::vector< BNode* >::iterator injection_edge = donor_nodes.begin() ; injection_edge != donor_nodes.end(); ++injection_edge){
            
            BNode* donor = (*injection_edge);

            double dx_inj = 0.0;
            double dy_inj = 0.0;
            double dz_inj = 0.0;

            // rate: inject_node -> donor 
            //double rate_inject_hole = 10E17;
            double cal_rate12e = 0.00;
            double cal_rate12h = 0.00;
            double cal_dexter = 1E6;
            double cal_fret = 1E6;
            double rate_inj = 1E6;
            double rate_col = 0.00;
            double rate_return = 0.00;
            
            votca::tools::vec distanceID(dx_inj, dy_inj, dz_inj);
            
            if (int(nodes.size())-((inject->id)-1) == donor->id || int(nodes.size())-inject->id == donor->id || int(nodes.size())-(inject->id+1) == donor->id){
            //if (inject->id == donor->id){
                Edge* edge_injection = new Edge(inject, donor, distanceID, cal_rate12e, cal_rate12h, cal_dexter, cal_fret, rate_inj, rate_col, rate_return);
                inject->AddEdge( edge_injection );
                edges.push_back( edge_injection );
            }
            //std::cout << " inject rate added for " << inject->id << " & "  << donor->id << " at distance of " << 0.00 << std::endl; 
        }
    }
    
    // Acceptor collect pairs
    for (std::vector< BNode* >::iterator acc_col_edge = acceptor_nodes.begin() ; acc_col_edge != acceptor_nodes.end(); ++acc_col_edge){
            
        BNode* acceptor = (*acc_col_edge);
            
        //collecting events from the acceptors - Phosphorescence
        for (int collect = 1; collect <= int(collect_nodes.size()); ++collect){
        
            //int node_c = collect + 3460;
            BNode* collect_node = GetCollectNode(collect);
        
            //double distance_AC = 0.0;
        
            double dx_col = 0.0;
            double dy_col = 0.0;
            double dz_col = 0.0;
            
            votca::tools::vec distance_AC(dx_col, dy_col, dz_col);
            //Need a value for acceptor emission after FRET 
            //double FRET_emission_rate = 0.01;
        
            double cal_rate12e = 0.00;
            double cal_rate12h = 0.00;
            double cal_dexter = 0.00; //10x slower than fluorescence 
            double cal_fret = 1E7; //fluorescence
            double rate_inj = 0.00;
            double rate_return = 0.00;
            double rate_col = 1.0;
                  
            if (acceptor->id == collect_node->id){
                Edge* edgeAC = new Edge(acceptor, collect_node, distance_AC, cal_rate12e, cal_rate12h, cal_dexter, cal_fret, rate_inj, rate_col, rate_return); 
                acceptor->AddEdge(edgeAC);
                edges.push_back( edgeAC );
                //std::cout << " Acceptor collect rate added for " << acceptor->id << " & "  << collect_node->id << " at distance of " << 0.00 << std::endl;
            }           
        }
        
        //collecting events from the acceptors - after dexter  
        for (int collect = 1; collect <= int(collect_nodes.size()); ++collect){
        
            //int node_c = collect + 3460;
            BNode* collect_node = GetCollectNode(collect);
        
            //double distance_AC = 0.0;
        
            double dx_col = 0.0;
            double dy_col = 0.0;
            double dz_col = 0.0;
            
            votca::tools::vec distance_AC(dx_col, dy_col, dz_col);
            //Need a value for acceptor emission after FRET 
            //double FRET_emission_rate = 0.01;
        
            double cal_rate12e = 0.00;
            double cal_rate12h = 0.00;
            double cal_dexter = 1E6; //10x slower than fluorescence 
            double cal_fret = 0.00; //fluorescence
            double rate_inj = 0.00;
            double rate_return = 0;
            double rate_col = 3.0;
                  
            if (acceptor->id == collect_node->id){
                Edge* edgeAC = new Edge(acceptor, collect_node, distance_AC, cal_rate12e, cal_rate12h, cal_dexter, cal_fret, rate_inj, rate_col, rate_return); 
                acceptor->AddEdge(edgeAC);
                edges.push_back( edgeAC );
                //std::cout << " Acceptor collect rate added for " << acceptor->id << " & "  << collect_node->id << " at distance of " << 0.00 << std::endl;
            }           
        }
        
    }

    //Donor collect pairs - phosphorescence
    for (std::vector< BNode* >::iterator collection_edge = donor_nodes.begin() ; collection_edge != donor_nodes.end(); ++collection_edge){
            
        BNode* donor = (*collection_edge);
           
        //collecting events from the donors
        for (int collect = 1; collect <= int(collect_nodes.size()); ++collect){
        
            //int node_c = collect + 3460;
            BNode* collect_node = GetCollectNode(collect);
        
            double dx_col = 0.0;
            double dy_col = 0.0;
            double dz_col = 0.0;
            
            votca::tools::vec distance_DC(dx_col, dy_col, dz_col);
            
            //Need a value for acceptor emission after FRET 
            //double FRET_emission_rate = 0.01;
        
            double cal_rate12e = 0.00;
            double cal_rate12h = 0.00;
            double cal_dexter = 0.00;
            double cal_fret = 1.6E6; //phosphorescence 1.6 microseconds 
            double rate_inj = 0.00;
            double rate_col = 2;
            double rate_return = 0.00;
                  
            if (donor->id == int(nodes.size()-(collect_node->id-1))){
                Edge* edgeDC = new Edge(donor, collect_node, distance_DC, cal_rate12e, cal_rate12h, cal_dexter, cal_fret, rate_inj, rate_col, rate_return); 
                donor->AddEdge(edgeDC);
                edges.push_back( edgeDC );
                //std::cout << " Donor collect rate added for " << donor->id << " & "  << collect_node->id << " at distance of " << 0.00 << std::endl;
            }            
        }
    }
    
        
    for (std::vector< BNode* >::iterator collect_node = collect_nodes_begin() ; collect_node != collect_nodes_end(); ++collect_node){

        BNode* collect = GetCollectNode( (*collect_node)->id );
        
        for (std::vector< BNode* >::iterator return_edge = inject_nodes.begin() ; return_edge != inject_nodes.end(); ++return_edge){
            
            BNode*  re_inject = (*return_edge);
            
            double dx_return = 0.0;
            double dy_return = 0.0;
            double dz_return = 0.0;

            // rate: inject_node -> donor 
            double cal_rate12e = 0.00;
            double cal_rate12h = 0.00;
            double cal_dexter = 1E6;
            double cal_fret = 1E6;
            double rate_inj = 0.00;
            double rate_col = 0.00;
            double rate_return = 1E6;
            
            votca::tools::vec distance(dx_return, dy_return, dz_return);
               
            //if (re_inject->id == collect->id){
            Edge* edge_return = new Edge(collect, re_inject, distance, cal_rate12e, cal_rate12h, cal_dexter, cal_fret, rate_inj, rate_col, rate_return);
            collect->AddEdge( edge_return );
            edges.push_back( edge_return );
            //}
            //std::cout << " return rate added for " << collect->id << " & "  << re_inject->id << " at distance of " << 0.00 << std::endl;
             
        }
        
    }
    
    
    //works if the acceptors are listed first in the sql file, otherwise have to run through all nodes to find the acceptors 
    
    /*for (int a = 1; a <= acceptor_nodes.size(); ++a){
        BNode* node2 = GetNode(a);

        //node types: 1 = acceptor, 2 = matrix, 3 = donor
        int type2 = node2->type;
        
        //std::cout << " node a " << node2->id <<  " ACC_SIZE " << acceptor_nodes.size() << std::endl;
        
        //run through all the nodes + 1 for all nodes 2
        //int max_donor = nodes.size() - collect_nodes.size();
        //std::cout << "nodes size " << nodes.size() << std::endl;
        
        for (int b = 2; b <=nodes.size(); ++b){
            
            BNode* node1 = GetNode(b);
            int type1 = node1->type;
    
            
            //if the first node is an acceptor and the second node is a donor 
            if (type2 == 1 && type1 == 3){
                
                double pos_x_acceptor = node2->position.getX();
                double pos_y_acceptor = node2->position.getY();
                double pos_z_acceptor = node2->position.getZ();
                
                double pos_x_donor = node1->position.getX();
                double pos_y_donor = node1->position.getY();
                double pos_z_donor = node1->position.getZ();
                
                // periodic boundary conditions - should be read from state.sql
                double pbc_box_x = 14;
                double pbc_box_y = 14;
                double pbc_box_z = 14;
                
                //double pbc_distance = 16;
                //double half_pbc_distance = 8;
                
                double AD_x_distance = pos_x_acceptor - pos_x_donor;
                double AD_y_distance = pos_y_acceptor - pos_y_donor;
                double AD_z_distance = pos_z_acceptor - pos_z_donor;
                //Acceptor donor distance 
                double distance_AD = sqrt(((AD_x_distance)*(AD_x_distance))+((AD_y_distance)*(AD_y_distance))+((AD_z_distance)*(AD_z_distance)));
                //double distance_AD = distance_AD_nm * 1E-9; 
              
                double distance_AD_pbc_x = 0.00;
                double distance_AD_pbc_y = 0.00;
                double distance_AD_pbc_z = 0.00;
                
                if ( AD_x_distance > (0.5*pbc_box_x)){ distance_AD_pbc_x = abs(pbc_box_x - AD_x_distance);}
                if ( AD_y_distance > (0.5*pbc_box_y)){ distance_AD_pbc_y = abs(pbc_box_y - AD_y_distance);}
                if ( AD_z_distance > (0.5*pbc_box_z)){ distance_AD_pbc_z = abs(pbc_box_z - AD_z_distance);}
                //if (distance_AD > half_pbc_distance){ distance_AD_pbc = abs(pbc_distance - distance_AD); } 
                
                double distance_AD_pbc = sqrt(((distance_AD_pbc_x)*(distance_AD_pbc_x))+((distance_AD_pbc_y)*(distance_AD_pbc_y))+((distance_AD_pbc_z)*(distance_AD_pbc_z)));
                
                if (distance_AD <= dexter_cut_off){
                    
                    //double distance_AD_nm = distance_AD * 1e-9;
                    double dexter_rate = exp(-(distance_AD));
                    
                    double cal_rate12e = 0.00;
                    double cal_rate12h = 0.00;
                    double cal_dexter = dexter_rate;
                    double cal_fret = 0.00;
                    double cal_inject = 0.00;
                    double cal_collect = 0.00;
                    double cal_return = 0.00;
                    
                    Edge* edge12 = new Edge(node1, node2, distance_AD, cal_rate12e, cal_rate12h, cal_dexter, cal_fret, cal_inject, cal_collect, cal_return); 
                    node1->AddEdge(edge12);
                    edges.push_back( edge12 );
                    
                    //std::cout << " Dexter rate added for " << node1->id << " & "  << node2->id << " at distance of " << distance_AD << "nm " << std::endl;
                    //std::cout << "Rate: " << dexter_rate << std::endl;
                }
                
                if (distance_AD_pbc != 0.00 && distance_AD_pbc <= dexter_cut_off){
                    
                    //double distance_AD_pbc_nm = distance_AD_pbc *1e-9;
                    double dexter_rate = exp(-(distance_AD_pbc));
                    
                    double cal_rate12e = 0.00;
                    double cal_rate12h = 0.00;
                    double cal_dexter = dexter_rate;
                    double cal_fret = 0.00;
                    double cal_inject = 0.00;
                    double cal_collect = 0.00;
                    double cal_return = 0.00;
                    
                    Edge* edge12 = new Edge(node1, node2, distance_AD_pbc, cal_rate12e, cal_rate12h, cal_dexter, cal_fret, cal_inject, cal_collect, cal_return); 
                    node1->AddEdge(edge12);
                    edges.push_back( edge12 );
                    
                    //std::cout << " PBC Dexter rate added for " << node1->id << " & "  << node2->id << " at distance of " << distance_AD_pbc << "nm" << std::endl;
                    //std::cout << "Rate: " << dexter_rate << std::endl;
                }
                
                if (distance_AD > dexter_cut_off) {
                    
                    //double distance_AD_nm = distance_AD * 1E-9;
                    //double FRET_rate = R_FRET/(pow(distance_AD,6));
                    double fret = R_FRET/distance_AD_pbc;
                    double FRET_rate = (pow(fret,6));
                    
                    double cal_rate12e = 0.00;
                    double cal_rate12h = 0.00;
                    double cal_dexter = 0.00;
                    double cal_fret = FRET_rate;
                    double rate_inj = 0.00;
                    double rate_col = 0.00;
                    double rate_return = 0.00;
                    
                    Edge* edge12 = new Edge(node1, node2, distance_AD, cal_rate12e, cal_rate12h, cal_dexter, cal_fret, rate_inj, rate_col, rate_return);
                    node1->AddEdge(edge12);
                    edges.push_back( edge12 );
                    
                    //std::cout << " FRET rate added for " << node1->id << " & "  << node2->id << " at distance of " << distance_AD << "nm" << std::endl;
                    //std::cout << "Rate: " << FRET_rate << std::endl;
                }
                
                if (distance_AD_pbc != 0.00 && distance_AD_pbc > dexter_cut_off){
                    
                    //double distance_AD_pbc_nm = distance_AD_pbc*1E-9;
                    double fret = R_FRET/distance_AD_pbc;
                    double FRET_rate = (pow(fret,6));
                    
                    double cal_rate12e = 0.00;
                    double cal_rate12h = 0.00;
                    double cal_dexter = 0.00;
                    double cal_fret = FRET_rate;
                    double rate_inj = 0.00;
                    double rate_col = 0.00;
                    double rate_return = 0.00;
                    
                    Edge* edge12 = new Edge(node1, node2, distance_AD_pbc, cal_rate12e, cal_rate12h, cal_dexter, cal_fret, rate_inj, rate_col, rate_return);
                    node1->AddEdge(edge12);
                    edges.push_back( edge12 );
                    
                    //std::cout << " PBC FRET rate added for " << node1->id << " & "  << node2->id << " at distance of " << distance_AD_pbc << "nm" << std::endl;
                    //std::cout << "Rate: " << FRET_rate << std::endl;
                }
            }
     
        }*/
        
    
        /*for (int collect = 1; collect <= collect_nodes.size(); ++collect){
        
            //int node_c = collect + 3460;
            BNode* collect_node = GetCollectNode(collect);
        
            double distance_AC = 0.0;
        
            //Need a value for acceptor emission after FRET 
            //double FRET_emission_rate = 0.01;
        
            double cal_rate12e = 0.00;
            double cal_rate12h = 0.00;
            double cal_dexter = 0.00001;
            double cal_fret = 0.00001;
            double rate_inj = 0.00;
            double rate_col = 0.00001;
            double rate_return = 0.00;
                  
            //if (node2->id == collect_node->id){
            Edge* edge21 = new Edge(node2, collect_node, distance_AC, cal_rate12e, cal_rate12h, cal_dexter, cal_fret, rate_inj, rate_col, rate_return); 
            node2->AddEdge(edge21);
            edges.push_back( edge21 );
            //}
            //std::cout << " Collect rate added for " << node2->id << " & "  << collect_node->id << " at distance of " << 0.00 << std::endl;
        }

        //std::cout << " check end " << std::endl;
    }
    
    
    for (std::vector< BNode* >::iterator inject_node = inject_nodes_begin() ; inject_node != inject_nodes_end(); ++inject_node){

        BNode* inject = GetInjectNode( (*inject_node)->id );
    
        for (std::vector< BNode* >::iterator injection_edge = donor_nodes.begin() ; injection_edge != donor_nodes.end(); ++injection_edge){
            
            BNode* donor = (*injection_edge);
            
            double dx_inj = 0.0;
            double dy_inj = 0.0;
            double dz_inj = 0.0;

            // rate: inject_node -> donor 
            //double rate_inject_hole = 10E17;
            double cal_rate12e = 0.00;
            double cal_rate12h = 0.00;
            double cal_dexter = 0.00001;
            double cal_fret = 0.00001;
            double rate_inj = 0.00001;
            double rate_col = 0.00;
            double rate_return = 0.00;
            
            votca::tools::vec distanceID(dx_inj, dy_inj, dz_inj);
            
            if ((nodes.size()-((inject->id)-1)) == donor->id || nodes.size()-inject->id == donor->id || nodes.size()-(inject->id+1) == donor->id){
                Edge* edge_injection = new Edge(inject, donor, distanceID, cal_rate12e, cal_rate12h, cal_dexter, cal_fret, rate_inj, rate_col, rate_return);
                inject->AddEdge( edge_injection );
                edges.push_back( edge_injection );
            }
            //std::cout << " inject rate added for " << inject->id << " & "  << injection->id << " at distance of " << 0.00 << std::endl;
 
        }
    }
    
    for (std::vector< BNode* >::iterator collect_node = collect_nodes_begin() ; collect_node != collect_nodes_end(); ++collect_node){

        BNode* collect = GetCollectNode( (*collect_node)->id );
        
        for (std::vector< BNode* >::iterator return_edge = inject_nodes.begin() ; return_edge != inject_nodes.end(); ++return_edge){
            
            BNode*  re_inject = (*return_edge);
            
            double dx_return = 0.0;
            double dy_return = 0.0;
            double dz_return = 0.0;

            // rate: inject_node -> donor 
            //double rate_inject_hole = 10E17;
            double cal_rate12e = 0.00;
            double cal_rate12h = 0.00;
            double cal_dexter = 0.00001;
            double cal_fret = 0.00001;
            double rate_inj = 0.00;
            double rate_col = 0.00;
            double rate_return = 0.00001;
            
            votca::tools::vec distance(dx_return, dy_return, dz_return);
               
            //if (re_inject->id == collect->id){
            Edge* edge_return = new Edge(collect, re_inject, distance, cal_rate12e, cal_rate12h, cal_dexter, cal_fret, rate_inj, rate_col, rate_return);
            collect->AddEdge( edge_return );
            edges.push_back( edge_return );
            //}
            //std::cout << " return rate added for " << collect->id << " & "  << re_inject->id << " at distance of " << 0.00 << std::endl;
             
        }
        
    }*/
    
    
    
}

void TerminalGraph::Print(){
    
    for (std::vector< BNode* >::iterator node = nodes.begin() ; node != nodes.end(); ++node) {
            (*node)->PrintNode();
    }
    for (std::vector< BNode* >::iterator node = collect_nodes.begin() ; node != collect_nodes.end(); ++node) {
        (*node)->PrintNode();
    }
    for (std::vector< BNode* >::iterator node = inject_nodes.begin() ; node != inject_nodes.end(); ++node) {
        (*node)->PrintNode();
    }    
};
 
    
}}



#endif

