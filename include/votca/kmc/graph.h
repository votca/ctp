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
    
    // iterator over carriers
    typedef std::vector< BNode* >::iterator iterator;
    typedef const std::vector< BNode* >::iterator const_iterator;
    
    iterator nodes_begin() { return nodes.begin(); }
    iterator nodes_end() { return nodes.end(); } 
    int nodes_size() {return nodes.size(); }
  
    void Load_Graph(std::string filename);
    void Load_Rates(std::string filename);
    void Rates_Calculation(std::string filename, int nelectrons, int nholes, double fieldX, double fieldY, double fieldZ, double temperature);
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
    std::vector< Edge* > edges;
    
};


void Graph::Load_Graph(std::string filename) {
    
    std::cout << "Loading the graph from " << filename << std::endl;
    
    // initialising the database file
    votca::tools::Database db;
    db.Open( filename );
      
    votca::tools::Statement *stmt = db.Prepare("SELECT id-1, posX, posY, posZ, UnCnNe, UnCnNh, UcNcCe, UcNcCh, eAnion, eNeutral, eCation, UcCnNe, UcCnNh FROM segments;");

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
        
        //node->PrintNode();   
    }
    
    delete stmt;
}

void Graph::Load_Rates(std::string filename){
    
    votca::tools::Database db;
    db.Open( filename );
    
    //List of neighbours (from neighbour list for charge transfer - state.sql "pairs" table) 
    //List of rates for electron transfer from seg 1 to seg 2 and vice versa
    
    votca::tools::Statement *stmt = db.Prepare("SELECT seg1, seg2, drX, drY, drZ, rate12e, rate21e, rate12h, rate21h FROM pairs ;");
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
       
        //std::cout << "seg1: " << seg1 << " seg2: " << seg2 << " rate12 " << rate12e << std::endl;
        
        votca::tools::vec distance_pbc(dx_pbc, dy_pbc, dz_pbc);
        Edge* edge12 = new Edge(node1, node2, distance_pbc, rate12e, rate12h); 
        Edge* edge21 = new Edge(node2, node1, -distance_pbc, rate21e, rate21h);
        
        edges.push_back( edge12 );
        edges.push_back( edge21 );
        
        // make sure no duplicates are added
        node1->AddEdge( edge12 );
        node2->AddEdge( edge21 );
        
         
    }
    delete stmt;  
}

void Graph::Rates_Calculation(std::string filename, int nelectrons, int nholes, double fieldX, double fieldY, double fieldZ, double temperature){
    
    double kB   = 8.617332478E-5; // eV/K
    double hbar = 6.5821192815E-16; // eV*s
    double Pi   = 3.14159265358979323846;
    
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
        
    }
}    

/*
double Graph::Determine_hopping_distance(vector<Node*> nodes) {
    
    //Determination of hopping distance
    
    double hopdistance = 0.0;
    
    for(unsigned int inode=0; inode < nodes.size(); inode++) {
        for(unsigned int ipair=0;ipair<nodes[inode]->static_event_info.size();ipair++) {
            
            myvec pairdistancevec = nodes[inode]->static_event_info[ipair].distance;
            double pairdistance = abs(pairdistancevec);
            if(pairdistance>hopdistance) {hopdistance = pairdistance;}
        }
    }
    
    return hopdistance;
}

*/
/*
myvec Graph::Determine_sim_box_size(vector<Node*> nodes) {
    
    //Determination of simulation box size
    //To do this, we first need to find a node with position vector a and pairing node with position vector b, such that
    //|a-b|>hopping distance
    //Note that it is possible that none of the pairs pass the simulation box boundaries
    //In this special case, we must determine the node with max x/y/z coordinate and min x/y/z coordinate
    
    bool bndcrosspairXfound = false;
    bool bndcrosspairYfound = false;
    bool bndcrosspairZfound = false;
    
    double sim_box_sizeX = 0.0;
    double sim_box_sizeY = 0.0;
    double sim_box_sizeZ = 0.0;
    
    double maxX = nodes[0]->node_position.x(); //initial values
    double maxY = nodes[0]->node_position.y();
    double maxZ = nodes[0]->node_position.z();
    double minX = nodes[0]->node_position.x(); //initial values
    double minY = nodes[0]->node_position.y();
    double minZ = nodes[0]->node_position.z();
    
    for(unsigned int inode=0;inode<nodes.size();inode++) {
        //Site energy difference for nodes 1 and 2 (for electrons and holes)
        //1->2
        double dG_Site_e12 = node2->site_energy_electron - node1->site_energy_electron;
        double dG_Site_h12 = node2->site_energy_hole - node1->site_energy_hole;
        //2->1
        double dG_Site_e21 = node1->site_energy_electron - node2->site_energy_electron;
        double dG_Site_h21 = node1->site_energy_hole - node2->site_energy_hole;
        
        //Site energy difference and field difference for charge carrier hop
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
        
        //std::cout << "Rate 12e " << cal_rate12e << "   Rate 21e: " << cal_rate21e << std::endl;
        
        //add the new edge with calculated rate
        Edge* edge12 = new Edge(node1, node2, distance, cal_rate12e, cal_rate12h);
        Edge* edge21 = new Edge(node2, node1, -distance, cal_rate21e, cal_rate21h);
 
        node1->AddEdge(edge12);
        node2->AddEdge( edge21 );

        edges.push_back( edge12 );
        edges.push_back( edge21 );
                  
    }
       
    delete stmt;
}
 */

void Graph::Print(){
    
        for (std::vector< BNode* >::iterator node = nodes.begin() ; node != nodes.end(); ++node) {
            (*node)->PrintNode();
        }
    };
 
    
}}



#endif

