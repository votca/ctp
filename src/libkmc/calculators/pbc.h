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

#ifndef __VOTCA_KMC_PBC_H
#define	__VOTCA_KMC_PBC_H

#include <votca/kmc/state.h>
#include <votca/kmc/event.h>
#include <votca/kmc/graph.h>
#include <votca/kmc/carrier.h>
#include <votca/kmc/bnode.h>
#include <votca/kmc/carrierfactory.h>
#include <votca/kmc/eventfactory.h>
#include "../algorithms/vssm2_nodes.h"
#include <fstream>

using namespace std;

namespace votca { namespace kmc {
   
class PBC : public KMCCalculator 
{
public:
    
    PBC() {};
   ~PBC() {};

    string  Identify() {return "pbc"; };
    using KMCCalculator::Initialize;
    void Initialize(Property *options);
    bool EvaluateFrame();

protected:
   void RunKMC(void); 
            
private:
    
    double _runtime;
    int _nsteps;
    int _seed;
    int _nelectrons;
    int _nholes;
    //std::string _injection_name;
    std::string _injection_method;
    double _fieldX;
    double _fieldY;
    double _fieldZ;
    myvec _field;
    double _temperature;
    double _outtime;
    std::string _trajectoryfile;
    std::string _rates;
    std::string _interaction;
};

void PBC::Initialize(Property *options) {
    
    std::cout << endl << "Initialising KMC with periodic boundary conditions" << endl;

    // update options with the VOTCA SHARE defaults - xml   
    UpdateWithDefaults( options );
    string key = "options." + Identify();
    
    _runtime = options->get(key + ".runtime").as<double>();
    _nsteps = options->get(key + ".nsteps").as<int>();
    _seed = options->get(key + ".seed").as<int>();
    _nelectrons = options->get(key + ".nelectrons").as<int>();
    _nholes = options->get(key + ".nholes").as<int>();
    //_injection_name = options->get(key + ".injection").as<string>();
    _injection_method = options->get(key + ".injectionmethod").as<string>();
    _fieldX = options->get(key + ".fieldX").as<double>();
    _fieldY = options->get(key + ".fieldY").as<double>();
    _fieldZ = options->get(key + ".fieldZ").as<double>();
    _field = myvec(_fieldX,_fieldY,_fieldZ);
    _temperature = options->get(key + ".temperature").as<double>();
    _outtime = options->get(key + ".outtime").as<double>();
    _trajectoryfile = options->get(key + ".trajectoryfile").as<string>();
    _rates = options->get(key + ".rates").as<string>();
    _interaction = options->get(key + ".interaction").as<string>();

}

bool PBC::EvaluateFrame() {
        
    RunKMC();
    return true;
}

void PBC::RunKMC() {

    
    std::cout << "Running KMC-PBC: " << std::endl;
   
    Graph graph;
    State state;
 
    //Load the graph from the state file
    std::string filename( "state.sql" );
    graph.Load_Graph( filename );
    
    //Interaction still to be implemented - only Pauli exclusion available right now
    if (_interaction == "Pauli"){
        std::cout << "Pauli exclusion principal used as method of interaction for like charges (no double occupation)" << std::endl;
    }
    /*else if (_interaction == "Coulomb"){
        std::cout << "  Coulomb electrostatic interaction between like charges included " << std::endl;
    }*/
    
    //option to read the rates from the state file or calculate
    if (_rates == "read"){
        graph.Load_Rates(filename);
    }
    else if ( _rates == "calculate"){
        graph.Rates_Calculation(filename, _nelectrons, _nholes, _fieldX, _fieldY, _fieldZ, _temperature);
    }
    else {
        std::cout << "The option for rates was incorrectly specified. Please choose to 'read' rates or 'calculate' rates. " << std::endl;
    }
 
    //graph.Print();
    CarrierFactory::RegisterAll();
    EventFactory::RegisterAll();
    
    std::cout << "Number of Nodes: " << graph.nodes_size() << std::endl;
    std::cout << "Number of electrons: " << _nelectrons << std::endl;
    std::cout << "Number of holes: " << _nholes << std::endl;
    std::cout << "Method of carrier injection: " << _injection_method << std::endl;
    
    std::cout << std::endl;
    
    if(_nelectrons>graph.nodes_size() || _nholes>graph.nodes_size()){
        std::cout << "The number of carriers exceeds the number of available nodes!" << std::endl;
        return;
    }
    
    // use the seed to initialise the random number generator srand
    srand(_seed);
    votca::tools::Random2 *RandomVariable = new votca::tools::Random2();
    RandomVariable->init(rand(), rand(), rand(), rand());
    
    std::cout << std::endl;
       
    //create electrons and place on nodes of the graph
    if(_nelectrons != 0){
 
        for ( int electron = 1; electron <= _nelectrons; ++electron ) {
            
            // Create electrons
            Carrier* e_carrier =  state.AddCarrier( "electron" );
            Electron* ecarrier = dynamic_cast<Electron*>(e_carrier);
            
            //randomly inject to a node in the graph
            if (_injection_method == "random"){ 
                int node_id = RandomVariable->rand_uniform_int(graph.nodes_size());
                BNode* node_from = graph.GetNode(node_id + 1);
                //carrier can only be placed if the node is free, else find a new node
                while (ecarrier->AddNode(node_from)==false){
                        int node_id = RandomVariable->rand_uniform_int(graph.nodes_size());
                        node_from = graph.GetNode(node_id + 1);
                    } 
                if (ecarrier->AddNode(node_from)==true){ ecarrier->AddNode( node_from );}
            }
            
            //inject to the first nodes of the graph to have an ordered placement of electrons 
            else if (_injection_method == "uniform") {
                BNode* node_from = graph.GetNode(electron);
                ecarrier->AddNode( node_from );
                //node_from->PrintNode();  
            }
        }
        
        std::cout << std::endl;
        
    }
    
    if(_nholes != 0){
        
        for ( int hole = 1; hole <= _nholes; ++hole ) {
            
            // Create holes
            Carrier* h_carrier =  state.AddCarrier( "hole" );
            Hole* hcarrier = dynamic_cast<Hole*>(h_carrier);
            
            //randomly inject to a node in the graph
            if (_injection_method == "random"){ 
                int node_id = RandomVariable->rand_uniform_int(graph.nodes_size());
                BNode* node_from = graph.GetNode(node_id + 2);
                //only place the carrier if the node is free, otherwise find a new node
                while (hcarrier->AddNode(node_from)==false){
                        int node_id = RandomVariable->rand_uniform_int(graph.nodes_size());
                        node_from = graph.GetNode(node_id + 2);
                    } 
                if (hcarrier->AddNode(node_from)==true){ hcarrier->AddNode( node_from );}
            }
            
            //inject to the first nodes of the graph
            else if (_injection_method == "uniform") {
                BNode* node_from = graph.GetNode(hole);
                hcarrier->AddNode( node_from );
                //node_from->PrintNode();  
            }
            
        }
        
        std::cout << std::endl;
    }
      
    //use vssm2_nodes to organise all the nodes and events 
    VSSM2_NODES vssm2;
    //initialise with the state and the graph 
    vssm2.Initialize( &state, &graph );
    //vssm2.AttachObserver(Observer, _nsteps );
    // run the algorithm
    vssm2.Run(_runtime, _nsteps, RandomVariable, _nelectrons, _nholes, _trajectoryfile, _outtime, _fieldX, _fieldY, _fieldZ);
    
}

}}


#endif	/* __VOTCA_KMC_PBC_H */
