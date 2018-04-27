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

#ifndef __VOTCA_KMC_VSSM2_EXCITED_H_
#define __VOTCA_KMC_VSSM2_EXCITED_H_

#include <unordered_map>
#include <time.h>
#include <votca/kmc/terminal_algorithm.h>
#include "../events/carrier_escape.h"
#include <votca/kmc/bnode.h>
#include "../events/dexter_energy_transfer.h"
#include "../events/forster_energy_transfer.h"


/* Two-level VSSM algorithm with nodes at the top level and reactions at the bottom level
//          head
//        /  |  \
//  node_1      node_n   
//  /  |  \
// move_1  move_k
*/

namespace votca { namespace kmc {
  
class VSSM2_EXCITED : public TerminalAlgorithm {
    
public:
  
/*void progressbar(double fraction)
{
    int totalbars = 50;
    std::cout << "\r";
    for(double bars=0; bars<double(totalbars); bars++)
    {
        if(bars<=fraction*double(totalbars))
        {
            std::cout << "|";
        }
        else
        {
            std::cout << "-";
        }
    }
    std::cout << "  " << int(fraction*1000)/10. <<" %   ";
    std::cout << std::flush;
    if(fraction*100 == 100)
    {
        std::cout << std::endl;
    }
}*/


void Initialize ( State* _state, TerminalGraph* _graph ) { 
    
    state = _state;
    terminalgraph = _graph;
    
    // Map of charge transfer events associated with a particular node
    std::unordered_map< BNode*, std::vector<Event*> > dexter_energy_transfer_map;
    std::unordered_map< BNode*, std::vector<Event*> > forster_energy_transfer_map;
    
    // Vector of all charge transfer events
    std::vector<DexterEnergyTransfer*> dexter_events;
    std::vector<ForsterEnergyTransfer*> fret_events;
    
    // Loop over all source nodes - injection events
    for (TerminalGraph::iterator it_inject_node = _graph->inject_nodes_begin(); it_inject_node != _graph->inject_nodes_end(); ++it_inject_node) {
        
        BNode* node_from = *it_inject_node;
        
        // create a new key with an empty vector
        forster_energy_transfer_map.emplace(node_from, vector<Event*>() );
        dexter_energy_transfer_map.emplace(node_from, vector<Event*>() );
        
        // Loop over all neighbours (edges) of the node
        for (BNode::EdgeIterator it_edge = node_from->EdgesBegin(); it_edge != node_from->EdgesEnd(); ++it_edge) {
                    
            Event* event_move_energy = Events().Create("forster_energy_transfer");
            ForsterEnergyTransfer* forster_energy_transfer = dynamic_cast<ForsterEnergyTransfer*> (event_move_energy);
            forster_energy_transfer->Initialize(NULL, *it_edge);
                        
            // Add a list of hole transfer events to the map, indexed by a node pointer
            forster_energy_transfer_map.at(node_from).push_back(event_move_energy);
            
            // Add an event to the energy transfer events
            fret_events.push_back(forster_energy_transfer);
            //std::cout << "Energy transfer created " << node_from->id << " -> " << (*it_edge)->NodeTo()->id << std::endl;
            
            Event* event_move_dexter_energy = Events().Create("dexter_energy_transfer");
            DexterEnergyTransfer* dexter_energy_transfer = dynamic_cast<DexterEnergyTransfer*> (event_move_dexter_energy);
            dexter_energy_transfer->Initialize(NULL, *it_edge);
                        
            // Add a list of energy transfer events to the map, indexed by a node pointer
            dexter_energy_transfer_map.at(node_from).push_back(event_move_dexter_energy);
            
            // Add an event to the energy transfer events
            dexter_events.push_back(dexter_energy_transfer);          
        }             
    }
    
    // Create all possible transfer events and associate them with the nodes
    for (TerminalGraph::iterator it_node = _graph->nodes_begin(); it_node != _graph->nodes_end(); ++it_node) {
        
        BNode* node_from = *it_node;
        
        // create a new key with an empty vector
        dexter_energy_transfer_map.emplace(node_from, vector<Event*>() );
        forster_energy_transfer_map.emplace(node_from, vector<Event*>() );
        //fret_collect_map.emplace(node_from, vector<Event*>() );
        
        // Loop over all neighbors (edges) of the node 
        for (BNode::EdgeIterator it_edge = node_from->EdgesBegin(); it_edge != node_from->EdgesEnd(); ++it_edge) {

            
            Event* event_move_energy = Events().Create("forster_energy_transfer");
            ForsterEnergyTransfer* forster_energy_transfer = dynamic_cast<ForsterEnergyTransfer*> (event_move_energy);
            forster_energy_transfer->Initialize(NULL, *it_edge);
                        
            // Add a list of energy transfer events to the map, indexed by a node pointer
            forster_energy_transfer_map.at(node_from).push_back(event_move_energy);
            
            // Add an event to the energy transfer events
            fret_events.push_back(forster_energy_transfer);
            //std::cout << "Energy transfer created " << node_from->id << " -> " << (*it_edge)->NodeTo()->id << std::endl;
            
            Event* event_move_dexter_energy = Events().Create("dexter_energy_transfer");
            DexterEnergyTransfer* dexter_energy_transfer = dynamic_cast<DexterEnergyTransfer*> (event_move_dexter_energy);
            dexter_energy_transfer->Initialize(NULL, *it_edge);
                        
            // Add a list of energy transfer events to the map, indexed by a node pointer
            dexter_energy_transfer_map.at(node_from).push_back(event_move_dexter_energy);
            
            // Add an event to the energy transfer events
            dexter_events.push_back(dexter_energy_transfer);        
         
        }
    }
    
    //Loop over all drain nodes - collection events and/or return to source events (closed circuit)
    for (TerminalGraph::iterator it_collect_node = _graph->collect_nodes_begin(); it_collect_node != _graph->collect_nodes_end(); ++it_collect_node) {
        
        BNode* node_from = *it_collect_node;
        
        dexter_energy_transfer_map.emplace(node_from, vector<Event*>() );
        forster_energy_transfer_map.emplace(node_from, vector<Event*>() ); 
        //fret_collect_map.emplace(node_from, vector<Event*>() );

        for (BNode::EdgeIterator it_edge = node_from->EdgesBegin(); it_edge != node_from->EdgesEnd(); ++it_edge) {
            
            Event* event_move_energy = Events().Create("forster_energy_transfer");
            ForsterEnergyTransfer* forster_energy_transfer = dynamic_cast<ForsterEnergyTransfer*> (event_move_energy);
            forster_energy_transfer->Initialize(NULL, *it_edge);
                        
            // Add a list of energy transfer events to the map, indexed by a node pointer
            forster_energy_transfer_map.at(node_from).push_back(event_move_energy);
            
            // Add an event to the energy transfer events
            fret_events.push_back(forster_energy_transfer);
            //std::cout << "Energy transfer created " << node_from->id << " -> " << (*it_edge)->NodeTo()->id << std::endl;
            
            Event* event_move_dexter_energy = Events().Create("dexter_energy_transfer");
            DexterEnergyTransfer* dexter_energy_transfer = dynamic_cast<DexterEnergyTransfer*> (event_move_dexter_energy);
            dexter_energy_transfer->Initialize(NULL, *it_edge);
                        
            // Add a list of energy transfer events to the map, indexed by a node pointer
            dexter_energy_transfer_map.at(node_from).push_back(event_move_dexter_energy);
            
            // Add an event to the energy transfer events
            dexter_events.push_back(dexter_energy_transfer);

        }             
    }
    
    // for every event, add a list of "events-to-enable" after OnExecute
    // and a list of "events-to-disable" after OnExecute
    for (auto& event: fret_events ) {
        BNode* node_from = event->NodeFrom();
        BNode* node_to = event->NodeTo();
        
        std::vector<Event*> events_to_disable = forster_energy_transfer_map.at(node_from);
        std::vector<Event*> events_to_enable = forster_energy_transfer_map.at(node_to);

        event->AddDisableOnExecute(&events_to_disable);
        event->AddEnableOnExecute(&events_to_enable);
    }
    
    for (auto& event:dexter_events ) {
        BNode* node_from = event->NodeFrom();
        BNode* node_to = event->NodeTo();
        
        std::vector<Event*> events_to_disable = dexter_energy_transfer_map.at(node_from);
        std::vector<Event*> events_to_enable = dexter_energy_transfer_map.at(node_to);

        event->AddDisableOnExecute(&events_to_disable);
        event->AddEnableOnExecute(&events_to_enable);
    }

    // for every event associated with node_from - create a list of events to check after onExecute
    // Events to check are every event for each node_to
    for (auto& event: fret_events ) {
        
        BNode* node_from = event->NodeFrom();
        std::vector<Event*> fret_events = forster_energy_transfer_map.at(node_from);
        
        for (std::vector<Event*>::iterator it_event = fret_events.begin(); it_event != fret_events.end(); ++it_event) {          
            Event* event_move_energy = *it_event;
            ForsterEnergyTransfer* forster_energy_transfer = dynamic_cast<ForsterEnergyTransfer*> (event_move_energy);
            
            BNode* node_to = forster_energy_transfer->NodeTo();
            std::vector<Event*> events_to_check = forster_energy_transfer_map.at(node_to);
            event->CheckEventsOnExecute(&events_to_check);
            
        }
    }
    
    for (auto& event:dexter_events ) {
        
        BNode* node_from = event->NodeFrom();
        std::vector<Event*>dexter_events = dexter_energy_transfer_map.at(node_from);
        
        for (std::vector<Event*>::iterator it_event =dexter_events.begin(); it_event !=dexter_events.end(); ++it_event) {
            Event* event_move_dexter_energy = *it_event;
            DexterEnergyTransfer* dexter_energy_transfer = dynamic_cast<DexterEnergyTransfer*> (event_move_dexter_energy);
            
            BNode* node_to = dexter_energy_transfer->NodeTo();
            std::vector<Event*> events_to_check = dexter_energy_transfer_map.at(node_to);
            
            event->CheckEventsOnExecute(&events_to_check);
        }
    }
    
    // organize events in a tree;
    // first level VSSM events (escape event for each carrier))
    for (State::iterator carrier = _state->begin(); carrier != _state->end(); ++carrier) {
        
        //std::cout << "Adding escape event for carrier " << (*carrier)->Type() << ", id " << (*carrier)->id() << std::endl;
         
        BNode* node_from = (*carrier)->GetNode();
        
        Event* event_escape = Events().Create("carrier_escape");
        CarrierEscape* carrier_escape = dynamic_cast<CarrierEscape*> (event_escape);
        carrier_escape->Initialize((*carrier));
        head_event.AddSubordinate( event_escape );
 
        std::vector<Event*> fret_events = forster_energy_transfer_map.at(node_from);        
        std::vector<Event*>dexter_events = dexter_energy_transfer_map.at(node_from);
        
        // Add move events from the map 
        for (std::vector<Event*>::iterator it_fret_event = fret_events.begin(); it_fret_event != fret_events.end(); ++it_fret_event) {

            // New event - energy transfer
            if ((*carrier)->Type() == "energy"){
                Event* event_move_energy = *it_fret_event;
                ForsterEnergyTransfer* forster_energy_transfer = dynamic_cast<ForsterEnergyTransfer*> (event_move_energy);
                Energy* energy = dynamic_cast<Energy*> ((*carrier));
                forster_energy_transfer->SetEnergy( energy );
                forster_energy_transfer->Enable();
                carrier_escape->AddSubordinate( event_move_energy );  
            }
        }
        
        for (std::vector<Event*>::iterator it_det_event = dexter_events.begin(); it_det_event != dexter_events.end(); ++it_det_event) {
            
            // New event - energy transfer
            if ((*carrier)->Type() == "energy"){
                Event* event_move_dexter_energy = *it_det_event;
                DexterEnergyTransfer* dexter_energy_transfer = dynamic_cast<DexterEnergyTransfer*> (event_move_dexter_energy);
                Energy* energy = dynamic_cast<Energy*> ((*carrier));
                dexter_energy_transfer->SetEnergy( energy );
                dexter_energy_transfer->Enable();
                carrier_escape->AddSubordinate( event_move_dexter_energy );  
            }
        } 
       
    }
    head_event.Enable();

}

void Run( double runtime, int nsteps, votca::tools::Random2 *RandomVariable, int nexciteddonor, int nexcitedacc, string trajectoryfile, double outtime, double fieldX, double fieldY, double fieldZ) {

    //votca::tools::Random2 RandomVariable;

    std::cout << std::endl << "Starting the KMC loop" << std::endl;
   
    clock_t begin = clock();

    // Initialise random number generator
    //srand(seed);
    //RandomVariable.init(rand(), rand(), rand(), rand());
    
    double time = 0.0;
    int step = 0;
    double trajout = 0.0;
    
    if ( runtime != 0 && nsteps == 0 ){ 
        runtime = runtime;
        std::cout << "Specified runtime (s): " << runtime << std::endl; 
    }
    else if ( nsteps != 0 && runtime == 0 ){ 
        nsteps = nsteps; 
        std::cout << "Specified number of simulation steps: " << nsteps << std::endl;
    }
    else if ( runtime !=0 && nsteps !=0 ){ 
        nsteps = nsteps; runtime = 0.0; 
        std::cout << "Specified both the number of simulation steps and the runtime -> number of steps will be used. " << std::endl; 
        std::cout << "Specified number of simulation steps: " << nsteps << std::endl;
    }
    
    state->Trajectory_create(trajectoryfile);
    state->Trajectory_write(trajout, trajectoryfile);
    
    trajout = outtime;
    
    //int energy_collected = state->Collect_Dexter.size() + state->Collect_Fluorescence.size() + state->Collect_Phosphorescence.size();
    while ( step < nsteps || time < runtime ){  
        
        //head_event.Print();
             
                
        double u = 1.0 - RandomVariable->rand_uniform();
        while(u == 0.0){ u = 1.0 - RandomVariable->rand_uniform();}
        //std::cout << " u " << u << std::endl;
        double elapsed_time = -1.0 / head_event.CumulativeRate() * log(u);
        //std::cout << " Rate " << head_event.CumulativeRate() << std::endl;
        //std::cout << " elapsed time " << elapsed_time << std::endl;
        state->AdvanceClock(elapsed_time);
        time += elapsed_time;
        step++;
        std::cout << "Time: " << time << std::endl;
        
        head_event.OnExecute(state, RandomVariable ); 
        //std::cout << "Time: " << time << std::endl;
                
        if (outtime != 0 && trajout < time )
        { 
            state->Trajectory_write(trajout, trajectoryfile);
            trajout = time + outtime;
        }
        

    }
    state->Print_excited_properties();
    clock_t end = clock();    
    printf("Elapsed: %f seconds after %i steps \n", (double)(end - begin) / CLOCKS_PER_SEC, step);
     
}

private:
    
    // head event, contains all escape events
    CarrierEscape head_event;
    // logger : move logger.h to tools
    // Logger log;   
};
    
}}



#endif

