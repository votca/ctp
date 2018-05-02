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

#ifndef __VOTCA_KMC_EXCITED_H
#define	__VOTCA_KMC_EXCITED_H

#include <votca/kmc/state.h>
#include <votca/kmc/event.h>
#include <votca/kmc/slabgraph.h>
#include <votca/kmc/carrier.h>
#include <votca/kmc/bnode.h>
#include <votca/kmc/carrierfactory.h>
#include <votca/kmc/eventfactory.h>
#include "../algorithms/vssm2_excited.h"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

namespace votca { namespace kmc {
   
class Excited : public KMCCalculator 
{
public:
    
    //Excited: excited state energy transfer - hops between triplet-1(D) and triplet-1(A) (DEXTER) or triplet-1(D) and singlet-1(A) (FRET)
    Excited() {};
   ~Excited() {};

    string  Identify() {return "excited"; };
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
    //int _nelectrons;
    //int _nholes;
    int _nexciteddonor;
    int _nexcitedacc;
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

void Excited::Initialize(Property *options) {
    
    std::cout << endl << "Initialising KMC excited" << endl;

    // update options with the VOTCASHARE defaults   
    UpdateWithDefaults( options );
    string key = "options." + Identify();
    
    _runtime = options->get(key + ".runtime").as<double>();
    _nsteps = options->get(key + ".nsteps").as<int>();
    _outtime = options->get(key + ".outtime").as<double>();
    _trajectoryfile = options->get(key + ".trajectoryfile").as<string>();
    _seed = options->get(key + ".seed").as<int>();
    //_nelectrons = options->get(key + ".nelectrons").as<int>();
    //_nholes = options->get(key + ".nholes").as<int>();
    _nexciteddonor = options->get(key + ".nexciteddonor").as<int>();
    _injection_name = options->get(key + ".injection").as<string>();
    _fieldX = options->get(key + ".fieldX").as<double>();
    _fieldY = options->get(key + ".fieldY").as<double>();
    _fieldZ = options->get(key + ".fieldZ").as<double>();
    _field = myvec(_fieldX,_fieldY,_fieldZ);
    _temperature = options->get(key + ".temperature").as<double>();
    _rates = options->get(key + ".rates").as<string>();

}

bool Excited::EvaluateFrame() {
        
    RunKMC();
    return true;
}

void Excited::RunKMC() {

    //votca::tools::Random2 RandomVariable;
    srand(_seed); 
    votca::tools::Random2 *RandomVariable = new votca::tools::Random2();
    RandomVariable->init(rand(), rand(), rand(), rand());
    
    std::cout << "Running KMC excited" << endl;
   
    SlabGraph graph;
    State state;
     
    std::string filename( "state.sql" );
    
    
    graph.Load_Excited_Graph(filename);
    graph.Create_excited_inject_collect_nodes(_nexciteddonor);
    graph.Load_Excited_pairs(filename);
    graph.Excited_energy_transfer_with_inject_collect(filename);
    
    /*if (_rates == "read"){
        std::cout << "Reading pairs from " << filename << std::endl;
        graph.Load_Excited_pairs_calculate_rates_loop(filename);
    }
    else if ( _rates == "calculate"){
        graph.Excited_energy_transfer_rates_and_pairs_calculation(filename);
    }
    else if (_rates == "snapshot"){
        graph.Load_Excited_pairs_calculate_rates_snapshot(filename);
    }
    else {
        std::cout << "Error: The option for rates was incorrectly specified. Please choose to 'read' rates or 'calculate' rates. " << std::endl;
    }*/

    CarrierFactory::RegisterAll();
    EventFactory::RegisterAll();
    
    std::cout << std::endl << "Number of nodes: " << graph.nodes_size() << std::endl;
    std::cout << "Number of donor molecules: " << graph.donor_nodes_size() <<  std::endl;
    std::cout << "Number of acceptor molecules: " << graph.acceptor_nodes_size() <<  std::endl;
    //std::cout << "Number of electrons: " << _nelectrons << std::endl;
    //std::cout << "Number of holes: " << _nholes << std::endl;
   
    
    //graph.Print();
    
    _nexcitedacc = 0;
    
    if(_nexciteddonor != 0){
        
        //srand(_seed);
        //RandomVariable.init(rand(), rand(), rand(), rand());
        std::cout << std::endl;
        
        for ( int excited = 1; excited <= _nexciteddonor; ++excited ) {
            
            // Create electrons
            Carrier* eng_carrier =  state.AddCarrier( "energy" );
            Energy* engcarrier = dynamic_cast<Energy*>(eng_carrier);
            
            
            if (_injection_name == "donor") {
                
                //std::cout << " Electron - added TEST " << std::endl;
                //int node_id = _nexciteddonor;
                BNode* node_from = graph.GetInjectNode(excited);
                engcarrier->AddNode( node_from );
                
                
            }  
            
        }
        
        std::cout << std::endl;
        
    }
    
    VSSM2_EXCITED vssm2;
    vssm2.Initialize( &state, &graph );
    vssm2.Run(_runtime, _nsteps, RandomVariable, _nexciteddonor, _nexcitedacc, _trajectoryfile, _outtime, _fieldX, _fieldY, _fieldZ);
    
}

}}


#endif	/* __VOTCA_KMC_EXCITED_H */
