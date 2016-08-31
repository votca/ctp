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
    void PrintGraph(){};
    
    // add a node to the vector of Nodes
    BNode* AddNode() {
        BNode *node = new BNode();
        nodes.push_back( node );
        return node;
    };
    
    std::vector< BNode* > nodes;
    
private:
    
};


void Graph::Load(std::string filename) {
    
    std::cout << "Loading the graph from " << filename << std::endl;
    
    // initialising the database file
    votca::tools::Database db;
    db.Open( filename );
    
    votca::tools::Statement *stmt = db.Prepare("SELECT _id-1, posX, posY, posZ FROM segments;");

    int i=0;
    while (stmt->Step() != SQLITE_DONE)
    {
       
        BNode *node = AddNode();

        //cout << "id: " << stmt->Column<int>(0) << "; ";
        int id = stmt->Column<int>(0);
        node->id = id;
        
        // position in nm
        double x = stmt->Column<double>(1);
        double y = stmt->Column<double>(2);
        double z = stmt->Column<double>(3);
    
        //cout << "position: " <<  x << " " << y << " " << z << endl;  
        
        myvec position = myvec(x, y, z); 
        node->position = position;

    }
    delete stmt;
    
    //List of neighbours - pairs   
    int numberofpairs = 0;
    stmt = db.Prepare("SELECT _id, id, seg1, seg2 FROM pairs;");
    while (stmt->Step() != SQLITE_DONE)     
    {
        //Is node ID the same for both neighbours?
        cout << "id 1: " << stmt->Column<int>(0) << "; "; 
        int _id = stmt->Column<int>(0);
        cout << "id 2: " << stmt->Column<int>(1) << "; ";
        int id = stmt->Column<int>(1);

        
        int seg1 = stmt->Column<int>(2);
        int seg2 = stmt->Column<int>(3);
        
        cout << "Neighbour pairs: " << seg1 << " " << seg2 << endl;
        numberofpairs ++;
        
        //vector neighbours = vector (seg1, seg2);
        //node->neighbours = neighbours;
        
    }
    delete stmt;

/*    
    
    // Load pairs and rates
    
        double rate12 = stmt->Column<double>(2);

        myvec dr = myvec(stmt->Column<double>(3)*1E-9, stmt->Column<double>(4)*1E-9, stmt->Column<double>(5)*1E-9); // converted from nm to m
        double Jeff2 = stmt->Column<double>(6);
        double reorg_out = stmt->Column<double>(7); 
        node[seg1]->AddEvent(seg2,rate12,dr,Jeff2,reorg_out);
        numberofpairs ++;
    }    
    delete stmt;

    if(votca::tools::globals::verbose) { cout << "pairs: " << numberofpairs/2 << endl; }
    
    // Calculate initial escape rates !!!THIS SHOULD BE MOVED SO THAT IT'S NOT DONE TWICE IN CASE OF COULOMB INTERACTION!!!
    for(unsigned int i=0; i<node.size(); i++)
    {
        node[i]->InitEscapeRate();
    }
    return node;
*/    
    
}

}}



#endif

