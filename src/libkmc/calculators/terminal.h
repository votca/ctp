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
    double _temperature;
    double _outtime;
    std::string _trajectoryfile;
    std::string _rates;
    std::string _source_electrode;
    std::string _drain_electrode;
};

void Terminal::Initialize(Property *options) {
    
    std::cout << endl << "Initialising KMC terminal" << endl;

    // update options with the VOTCASHARE defaults   
    UpdateWithDefaults( options );
    string key = "options." + Identify();
    
    _runtime = options->get(key + ".runtime").as<double>();
    _nsteps = options->get(key + ".nsteps").as<int>();
    _outtime = options->get(key + ".outtime").as<double>();
    _trajectoryfile = options->get(key + ".trajectoryfile").as<string>();
    _seed = options->get(key + ".seed").as<int>();
    _nelectrons = options->get(key + ".nelectrons").as<int>();
    _nholes = options->get(key + ".nholes").as<int>();
    _injection_name = options->get(key + ".injection").as<string>();
    _fieldX = options->get(key + ".fieldX").as<double>();
    _fieldY = options->get(key + ".fieldY").as<double>();
    _fieldZ = options->get(key + ".fieldZ").as<double>();
    _field = myvec(_fieldX,_fieldY,_fieldZ);
    _temperature = options->get(key + ".temperature").as<double>();
    _rates = options->get(key + ".rates").as<string>();
    _source_electrode = options->get(key + ".source_electrode").as<string>();
    _drain_electrode = options->get(key + ".drain_electrode").as<string>();

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
    /*std::istringstream iss_source(_source_electrode); // position = "x,y,z" 
    double source_electrode_x, source_electrode_y, source_electrode_z;
    char delimiter_source;
    iss_source >> source_electrode_x >> delimiter_source >> source_electrode_y >> delimiter_source >> source_electrode_z;
    std::cout << "Source electrode created at position: (" << source_electrode_x  << "," << source_electrode_y << "," << source_electrode_z << ") " << std::endl;
    terminalgraph.Create_source_electrode( (_nelectrons + _nholes), source_electrode_x, source_electrode_y, source_electrode_z );

     */
    
    /*string field_direction = "";
    if(_fieldX != 0 && _fieldY==0 && _fieldZ==0) {field_direction = "X"; }
    else if(_fieldY != 0 && _fieldX==0 && _fieldZ==0) {field_direction = "Y"; }
    else if(_fieldZ != 0 && _fieldX==0 && _fieldY==0) {field_direction = "Z"; }
    */
    
    terminalgraph.Load_Graph(filename, _temperature);
        
    //terminalgraph.Load_injectable_collectable(field_direction);
  
    //create the drain electrode - one drain node per carrier
    /*std::istringstream iss_drain(_drain_electrode); // position = "x,y,z" 
    double drain_electrode_x, drain_electrode_y, drain_electrode_z;
    char delimiter_drain;
    iss_drain >> drain_electrode_x >> delimiter_drain >> drain_electrode_y >> delimiter_drain >> drain_electrode_z;
    std::cout << "Drain electrode created at position: (" << drain_electrode_x << "," << drain_electrode_y << "," << drain_electrode_z << ") "<< std::endl;
    terminalgraph.Create_drain_electrode( (_nelectrons + _nholes), drain_electrode_x, drain_electrode_y, drain_electrode_z );

    terminalgraph.Load_Electrode_Neighbours( filename );
    */
    
    if (_rates == "read"){
        std::cout << "Reading rates from " << filename << std::endl;
        terminalgraph.Load_Rates(filename);
    }
    else if ( _rates == "calculate"){
        terminalgraph.Rates_Calculation(filename, _nelectrons, _nholes, _fieldX, _fieldY, _fieldZ, _temperature);
    }
    else {
        std::cout << "Error: The option for rates was incorrectly specified. Please choose to 'read' rates or 'calculate' rates. " << std::endl;
    }
    
    //terminalgraph.Print();
    
    CarrierFactory::RegisterAll();
    EventFactory::RegisterAll();
    
    std::cout << std::endl << "Number of nodes: " << terminalgraph.nodes_size() << std::endl;
    //std::cout << "Number of source nodes (injection): " << terminalgraph.source_nodes_size() <<  std::endl;
    //std::cout << "Number of drain nodes (collection): " << terminalgraph.drain_nodes_size() <<  std::endl;
    std::cout << "Number of electrons: " << _nelectrons << std::endl;
    std::cout << "Number of holes: " << _nholes << std::endl;
   
    if(_nelectrons != 0){
        for ( int electron = 1; electron <= _nelectrons; ++electron ) {
            
            // Create electrons
            Carrier* e_carrier =  state.AddCarrier( "electron" );
            Electron* ecarrier = dynamic_cast<Electron*>(e_carrier);

            BNode* node_from = terminalgraph.GetNode(electron);
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
            
            BNode* node_from = terminalgraph.GetNode(hole);
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
