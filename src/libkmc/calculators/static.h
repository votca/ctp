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

#ifndef __VOTCA_KMC_STATIC_H
#define	__VOTCA_KMC_STATIC_H

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
   
class Static : public KMCCalculator 
{
public:
    
    Static() {};
   ~Static() {};

    string  Identify() {return "static"; };
    using KMCCalculator::Initialize;
    void Initialize(Property *options);
    bool EvaluateFrame();

protected:
   void RunKMC(void); 
            
private:
    
    double _runtime;
    int _nsteps;
    int _seedelectron;
    int _seedhole;
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
};

void Static::Initialize(Property *options) {
    
    std::cout << endl << "Initialising KMC static" << endl;

    // update options with the VOTCASHARE defaults   
    UpdateWithDefaults( options );
    string key = "options." + Identify();
    
    _runtime = options->get(key + ".runtime").as<double>();
    _nsteps = options->get(key + ".nsteps").as<int>();
    _seedelectron = options->get(key + ".seedelectron").as<int>();
    _seedhole = options->get(key + ".seedhole").as<int>();
    _seed = options->get(key + ".seed").as<int>();
    _nelectrons = options->get(key + ".nelectrons").as<int>();
    _nholes = options->get(key + ".nholes").as<int>();
    _injection_name = options->get(key + ".injection").as<string>();
    _injection_method = options->get(key + ".injectionmethod").as<string>();
    _fieldX = options->get(key + ".fieldX").as<double>();
    _fieldY = options->get(key + ".fieldY").as<double>();
    _fieldZ = options->get(key + ".fieldZ").as<double>();
    _field = myvec(_fieldX,_fieldY,_fieldZ);
    _temperature = options->get(key + ".temperature").as<double>();
    _outtime = options->get(key + ".outtime").as<double>();
    _trajectoryfile = options->get(key + ".trajectoryfile").as<string>();
    _rates = options->get(key + ".rates").as<string>();

}

bool Static::EvaluateFrame() {
        
    RunKMC();
    return true;
}

void Static::RunKMC() {

    votca::tools::Random2 RandomVariable;
    
    std::cout << "Running KMC static" << endl;
   
    Graph graph;
    State state;
 
    std::string filename( "state.sql" );
    graph.Load_Graph( filename );
    
    if (_rates == "read"){
        graph.Load_Rates(filename);
    }
    else if ( _rates == "calculate"){
        graph.Rates_Calculation(filename, _nelectrons, _nholes, _fieldX, _fieldY, _fieldZ, _temperature);
    }
    else {
        std::cout << "The option for rates was incorrectly specified. Please choose to 'read' rates or 'calculate' rates. " << std::cout;
    }
 
    //graph.Print();
    CarrierFactory::RegisterAll();

    EventFactory::RegisterAll();
    
    std::cout << "Number of Nodes: " << graph.nodes_size() << std::endl;
    std::cout << "Number of electrons: " << _nelectrons << std::endl;
    std::cout << "Number of holes: " << _nholes << std::endl;
    std::cout << "Method of carrier injection: " << _injection_method << std::endl;
    
    std::cout << std::endl;
    
    if(_nelectrons>graph.nodes_size()){
        std::cout << "The number of electrons exceeds the number of available nodes!" << std::endl;
        return;
    }
    
    if(_nelectrons != 0){
        
        srand(_seedelectron);
        RandomVariable.init(rand(), rand(), rand(), rand());
        std::cout << std::endl;
        
        for ( int electron = 1; electron <= _nelectrons; ++electron ) {
            
            // Create electrons
            Carrier* e_carrier =  state.AddCarrier( "electron" );
            Electron* ecarrier = dynamic_cast<Electron*>(e_carrier);
            
            if (_injection_method == "random"){ 
                int node_id = RandomVariable.rand_uniform_int(graph.nodes_size());
                BNode* node_from = graph.GetNode(node_id + 1);
                while (ecarrier->AddNode(node_from)==false){
                        int node_id = RandomVariable.rand_uniform_int(graph.nodes_size());
                        node_from = graph.GetNode(node_id + 1);
                    } 
                if (ecarrier->AddNode(node_from)==true){ ecarrier->AddNode( node_from );}
            }
            
            else if (_injection_method == "uniform") {
                BNode* node_from = graph.GetNode(2180 + electron);
                ecarrier->AddNode( node_from );
                //node_from->PrintNode();  
            }
        }
        
        std::cout << std::endl;
        
    }
    
    if(_nholes != 0){
               
        srand(_seedhole);        
        RandomVariable.init(rand(), rand(), rand(), rand());
        std::cout << std::endl;
        
        for ( int hole = 1; hole <= _nholes; ++hole ) {
            
            // Create holes
            Carrier* h_carrier =  state.AddCarrier( "hole" );
            Hole* hcarrier = dynamic_cast<Hole*>(h_carrier);
            
            if (_injection_method == "random"){ 
                int node_id = RandomVariable.rand_uniform_int(graph.nodes_size());
                BNode* node_from = graph.GetNode(node_id + 1);
                while (hcarrier->AddNode(node_from)==false){
                        int node_id = RandomVariable.rand_uniform_int(graph.nodes_size());
                        node_from = graph.GetNode(node_id + 1);
                    } 
                if (hcarrier->AddNode(node_from)==true){ hcarrier->AddNode( node_from );}
            }
            
            else if (_injection_method == "uniform") {
                BNode* node_from = graph.GetNode(1000 + hole);
                hcarrier->AddNode( node_from );
                //node_from->PrintNode();  
            }
            
        }
        
        std::cout << std::endl;
    }
      
    VSSM2_NODES vssm2;
    vssm2.Initialize( &state, &graph );
    //vssm2.AttachObserver(Observer, _nsteps );
    vssm2.Run(_runtime, _nsteps, _seed, _nelectrons, _nholes, _trajectoryfile, _outtime, _fieldX, _fieldY, _fieldZ);
    
}

}}


#endif	/* __VOTCA_KMC_STATIC_H */
