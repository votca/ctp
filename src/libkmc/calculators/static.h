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
};

void Static::Initialize(Property *options) {
    
    std::cout << endl << "Initialising KMC static" << endl;

    // update options with the VOTCASHARE defaults   
    UpdateWithDefaults( options );
    string key = "options." + Identify();
    
    _runtime = options->get(key + ".runtime").as<double>();
    _nsteps = options->get(key + ".nsteps").as<int>();
    _seed = options->get(key + ".seed").as<int>();
    _nelectrons = options->get(key + ".nelectrons").as<int>();
    //_nholes = options->get(key + ".nholes").as<int>();
    _injection_name = options->get(key + ".injection").as<string>();
    _injection_method = options->get(key + ".injectionmethod").as<string>();
    _fieldX = options->get(key + ".fieldX").as<double>();
    _fieldY = options->get(key + ".fieldY").as<double>();
    _fieldZ = options->get(key + ".fieldZ").as<double>();
    _field = myvec(_fieldX,_fieldY,_fieldZ);
    _outtime = options->get(key + ".outtime").as<double>();
    _trajectoryfile = options->get(key + ".trajectoryfile").as<string>();

}

bool Static::EvaluateFrame() {
        
    RunKMC();
    return true;
}

void Static::RunKMC() {

    std::cout << "Running KMC static" << endl;
    
    //For the random injection of electrons
    votca::tools::Random2 RandomVariable;
    srand(_seed);
    RandomVariable.init(rand(), rand(), rand(), rand());
    
    Graph graph;
    State state;
 
    std::string filename( "state.sql" );
    graph.Load( filename );
 
    CarrierFactory::RegisterAll();

    EventFactory::RegisterAll();
    
    std::cout << "Number of Nodes: " << graph.nodes_size() << std::endl;
    std::cout << "Number of electrons: " << _nelectrons << std::endl;
    //std::cout << "Number of holes: " << _nholes << std::endl;
    std::cout << "Method of carrier injection: " << _injection_method << std::endl << std::endl;
    
    if(_nelectrons>graph.nodes_size()){
        std::cout << "The number of electrons exceeds the number of available nodes!" << std::endl;
        return;
    }
    
    if(_nelectrons != 0){
        for ( int electron = 1; electron <= _nelectrons; ++electron ) {
            
            // Create electrons
            Carrier* carrier =  state.AddCarrier( "electron" );
            Electron* ecarrier = dynamic_cast<Electron*>(carrier);
                
            // randomly place the carrier on the node
            //BUG HERE - carrier being placed on occupied node
            //Have to check if the node is already occupied
            
            if (_injection_method == "random"){ 
                int node_id = RandomVariable.rand_uniform_int(graph.nodes_size());
                BNode* node_from = graph.GetNode(node_id + 1);
                while (ecarrier->AddNode(node_from)==false){
                        int node_id = RandomVariable.rand_uniform_int(graph.nodes_size());
                        node_from = graph.GetNode(node_id + 1);
                    } 
                if (ecarrier->AddNode(node_from)){ ecarrier->AddNode( node_from );}
            }               
                //node_from->PrintNode();  
            else if (_injection_method == "uniform") {
                BNode* node_from = graph.GetNode(2180 + electron);
                ecarrier->AddNode( node_from );
                //node_from->PrintNode();  
            }
        }
    }
       
    VSSM2_NODES vssm2;
    vssm2.Initialize( &state, &graph );
    //vssm2.AttachObserver(Observer, _nsteps );
    vssm2.Run(_runtime, _nsteps, _seed, _nelectrons, _trajectoryfile, _outtime, _fieldX, _fieldY, _fieldZ);
    
}

}}


#endif	/* __VOTCA_KMC_STATIC_H */
