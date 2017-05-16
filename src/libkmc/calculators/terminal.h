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

#ifndef __VOTCA_KMC_TERMINAL_H
#define	__VOTCA_KMC_TERMINAL_H

#include <votca/kmc/state.h>
#include <votca/kmc/event.h>
#include <votca/kmc/terminalgraph.h>
#include <votca/kmc/carrier.h>
#include <votca/kmc/bnode.h>
#include <votca/kmc/carrierfactory.h>
#include <votca/kmc/eventfactory.h>
#include "../algorithms/vssm2_terminal.h"
#include <fstream>

using namespace std;

namespace votca { namespace kmc {
   
class Terminal : public KMCCalculator 
{
public:
    
    Terminal() {};
   ~Terminal() {};

    string  Identify() {return "terminal"; };
    using KMCCalculator::Initialize;
    void Initialize(Property *options);
    bool EvaluateFrame();

protected:
   void RunKMC(void); 
            
private:
    
    double _runtime;
    int _nsteps;
    int _seedelectron;
    int _seed;
    int _nelectrons;
    int _nholes;
    std::string _injection_name;
    std::string _injection_method;
    double _fieldX;
    double _fieldY;
    double _fieldZ;
    myvec _field;
    double _outtime;
    std::string _trajectoryfile;
    std::string _source_electrode;
    std::string _drain_electrode;
    std::string _injection_cutoff;
    std::string _collection_cutoff;
};

void Terminal::Initialize(Property *options) {
    
    std::cout << endl << "Initialising KMC terminal" << endl;

    // update options with the VOTCASHARE defaults   
    UpdateWithDefaults( options );
    string key = "options." + Identify();
    
    _runtime = options->get(key + ".runtime").as<double>();
    _nsteps = options->get(key + ".nsteps").as<int>();
    _seed = options->get(key + ".seed").as<int>();
    _nelectrons = options->get(key + ".nelectrons").as<int>();
    _nholes = options->get(key + ".nholes").as<int>();
    _injection_name = options->get(key + ".injection").as<string>();
    _fieldX = options->get(key + ".fieldX").as<double>();
    _fieldY = options->get(key + ".fieldY").as<double>();
    _fieldZ = options->get(key + ".fieldZ").as<double>();
    _field = myvec(_fieldX,_fieldY,_fieldZ);
    _outtime = options->get(key + ".outtime").as<double>();
    _trajectoryfile = options->get(key + ".trajectoryfile").as<string>();
    _source_electrode = options->get(key + ".source_electrode").as<string>();
    _drain_electrode = options->get(key + ".drain_electrode").as<string>();
    _injection_cutoff = options->get(key + ".injection_cutoff").as<string>();
    _collection_cutoff = options->get(key + ".collection_cutoff").as<string>();
}

bool Terminal::EvaluateFrame() {
        
    RunKMC();
    return true;
}

void Terminal::RunKMC() {

    votca::tools::Random2 RandomVariable;
    
    std::cout << "Running KMC terminal" << endl;
   
    TerminalGraph terminalgraph;
    State state;
     
    std::string filename( "state.sql" );

    //create the source electrode - one source node per carrier id 0 to id (number of carriers)
    std::istringstream iss_source(_source_electrode); // position = "x,y,z" 
    double source_electrode_x, source_electrode_y, source_electrode_z;
    char delimiter_source;
    iss_source >> source_electrode_x >> delimiter_source >> source_electrode_y >> delimiter_source >> source_electrode_z;
    std::cout << "Source electrode created at position: (" << source_electrode_x  << "," << source_electrode_y << "," << source_electrode_z << ") " << std::endl;
    terminalgraph.Create_source_electrode( (_nelectrons + _nholes), source_electrode_x, source_electrode_y, source_electrode_z );

    //Define the injection face with the axis provided in the options
    std::istringstream iss_inject(_injection_cutoff); // position = "x,y,z" 
    double inject_x, inject_y, inject_z;
    char delimiter_inject;
    iss_inject>> inject_x >> delimiter_inject >> inject_y >> delimiter_inject >> inject_z;
    if(inject_x != 0 && inject_y==0 && inject_z==0) {
        std::cout << "Injection cut-off placed at x = " << inject_x << " all nodes with x less than or equal to " << inject_x << " will be injectable nodes." << std::endl;
    }
    else if(inject_y != 0 && inject_x==0 && inject_z==0) {
        std::cout << "Injection cut-off placed at y = " << inject_y << " all nodes with y less than or equal to " << inject_y << " will be injectable nodes." << std::endl;
    }
    else if(inject_z != 0 && inject_x==0 && inject_y==0) {
        std::cout << "Injection cut-off placed at z = " << inject_z << " all nodes with z less than or equal to " << inject_z << " will be injectable nodes." << std::endl;
    }
    else {
        std::cout << "Error with the injection cut-off (check that only one axis is specified)." << std::endl;
        return;
    }
    
    //Define the collection face with the axis provided in the option
    std::istringstream iss_collect(_collection_cutoff); // position = "x,y,z" 
    double collect_x, collect_y, collect_z;
    char delimiter_collect;
    iss_collect >> collect_x >> delimiter_collect >> collect_y >> delimiter_collect >> collect_z;
    if(collect_x != 0 && collect_y==0 && collect_z==0) {
        std::cout << "Collection cut-off placed at x = " << collect_x << " all nodes with x greater than or equal to " << collect_x << " will be collectable nodes." << std::endl;
    }
    else if(collect_y != 0 && collect_x==0 && collect_z==0) {
        std::cout << "Collection cut-off placed at y = " << collect_y << " all nodes with y greater than or equal to " << collect_y << " will be collectable nodes." << std::endl;
    }
    else if(collect_z != 0 && collect_x==0 && collect_y==0) {
        std::cout << "Collection cut-off placed at z = " << collect_z << " all nodes with z greater than or equal to " << collect_z << " will be collectable nodes." << std::endl;
    }
    else {
        std::cout << "Error with the collection cut-off (check that only one axis is specified)." << std::endl;
        return;
    }
    
    //load the graph
    //including the injectable region and the collectable region
    terminalgraph.Load_Graph( filename, inject_x, inject_y, inject_z, collect_x, collect_y, collect_z );

    //create the drain electrode - one drain node per carrier
    std::istringstream iss_drain(_drain_electrode); // position = "x,y,z" 
    double drain_electrode_x, drain_electrode_y, drain_electrode_z;
    char delimiter_drain;
    iss_drain >> drain_electrode_x >> delimiter_drain >> drain_electrode_y >> delimiter_drain >> drain_electrode_z;
    std::cout << "Drain electrode created at position: (" << drain_electrode_x << "," << drain_electrode_y << "," << drain_electrode_z << ") "<< std::endl;
    terminalgraph.Create_drain_electrode( (_nelectrons + _nholes), drain_electrode_x, drain_electrode_y, drain_electrode_z );

    terminalgraph.Load_Neighbours( filename );
    
    //terminalgraph.Print();
    CarrierFactory::RegisterAll();
    EventFactory::RegisterAll();
    
    std::cout << "Number of nodes: " << terminalgraph.lattice_nodes_size() << std::endl;
    std::cout << "Number of source nodes (injection): " << terminalgraph.source_nodes_size() <<  std::endl;
    std::cout << "Number of drain nodes (collection): " << terminalgraph.drain_nodes_size() <<  std::endl;
    std::cout << "Number of electrons: " << _nelectrons << std::endl;
    std::cout << "Number of holes: " << _nholes << std::endl;
   
    if(_nelectrons != 0){
        for ( int electron = 1; electron <= _nelectrons; ++electron ) {
            
            // Create electrons
            Carrier* e_carrier =  state.AddCarrier( "electron" );
            Electron* ecarrier = dynamic_cast<Electron*>(e_carrier);

            BNode* node_from = terminalgraph.GetSourceNode(electron);
            ecarrier->AddNode( node_from );
            //node_from->PrintNode();  
            
        }
    }
    
    std::cout << std::endl;
    
    if(_nholes != 0){
        for ( int hole = 1; hole <= _nholes; ++hole ) {
            
            // Create holes
            Carrier* h_carrier =  state.AddCarrier( "hole" );
            Hole* hcarrier = dynamic_cast<Hole*>(h_carrier);
            
            BNode* node_from = terminalgraph.GetSourceNode(hole);
            hcarrier->AddNode( node_from );
            //node_from->PrintNode();  
            
        }
    }
    
    VSSM2_TERMINAL vssm2;
    vssm2.Initialize( &state, &terminalgraph );
    vssm2.Run(_runtime, _nsteps, _seed, _nelectrons, _nholes, _trajectoryfile, _outtime, _fieldX, _fieldY, _fieldZ);
    
}

}}


#endif	/* __VOTCA_KMC_TERMINAL_H */
