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
    int _seed;
    int _nelectrons;
    std::string _injection_name;
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
    _seed = options->get(key + ".seed").as<int>();
    _nelectrons = options->get(key + ".nelectrons").as<int>();
    _injection_name = options->get(key + ".injection").as<string>();
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

    Graph graph;
    State state;

    std::string filename( "state.sql" );
    graph.Load( filename );
    //graph.Print();

    // register all carrier types
    CarrierFactory::RegisterAll();

    // register all event types
    EventFactory::RegisterAll();
    
    //Create a new electron
    Carrier* carrier =  state.AddCarrier( "electron" );
    Electron* electron = dynamic_cast<Electron*>(carrier);
    
    // place the electron on the first node
    BNode* node_from = graph.GetNode(1);
    carrier->SetNode( node_from );
    node_from->PrintNode();
   
    std::cout << "----" << std::endl;
    std::cout << "----" << std::endl;
    VSSM2_NODES vssm2;
    vssm2.Initialize( &state, &graph );
    vssm2.Run(0);
    std::cout << "----" << std::endl;
    std::cout << "----" << std::endl;
    
}

}}


#endif	/* __VOTCA_KMC_STATIC_H */
