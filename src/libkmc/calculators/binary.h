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

#ifndef __VOTCA_KMC_BINARY_H
#define	__VOTCA_KMC_BINARY_H

#include <votca/kmc/state.h>
#include <votca/kmc/event.h>
#include <votca/kmc/graph.h>
#include <votca/kmc/carrier.h>
#include <votca/kmc/bnode.h>
#include <votca/kmc/carrierfactory.h>
#include <votca/kmc/eventfactory.h>

#include "Events/electron_transfer.h" 

using namespace std;

namespace votca { namespace kmc {
   
class Binary : public KMCCalculator 
{
public:
    
    Binary() {};
   ~Binary() {};

    string  Identify() {return "binary"; };
    using KMCCalculator::Initialize;
    void Initialize(Property *options);
    bool EvaluateFrame();

protected:
   void RunKMC(void); 
            
private:
   
};

void Binary::Initialize(Property *options) {
    
    std::cout << "Hello from KMC binary" << endl;
    
}

bool Binary::EvaluateFrame() {
        
    RunKMC();
    return true;
}

void Binary::RunKMC() {

    std::cout << "Running KMC binary" << endl;

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
    Carrier* electron =  state.AddCarrier( "electron" );
    std::cout << electron->Type() << std::endl;
    
    // place the electron on the first node
    BNode* node_from = graph.GetNode(1);
    node_from->PrintNode();

    std::vector< Event* > events;

    for (BNode::iterator node_to = node_from->begin() ; node_to != node_from->end(); ++node_to) {
        
        //New event - electron transfer
        Event* _et =  Events().Create( "electron_transfer" );
        Electrontransfer* et = new Electrontransfer();
        
        et = dynamic_cast<Electrontransfer*>(_et);
        et->AddElectron( electron );
        et->SetOrigin( node_from );
        et->SetDestination( *node_to );
        
        // these are all transfer events originating from the node_to - they are disabled once the charge moves
        et->AddToToBeDisabled( _et );
        et->Enable();

        events.push_back( _et );
        _et->OnExecute( &state );

    }
    std::cout << "Number of events " << events.size() << std::endl;
 
    
    //Node_to becomes new node_from - disable the previous events and list the new events
    
}

}}


#endif	/* __VOTCA_KMC_BINARY_H */
