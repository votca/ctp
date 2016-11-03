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

#ifndef __VOTCA_KMC_EVENTS_UPDATE_TREE_H_
#define __VOTCA_KMC_EVENTS_UPDATE_TREE_H_

#include <votca/kmc/bnode.h>
#include <votca/kmc/state.h>
#include <votca/kmc/event.h>
#include "events/carrier_escape.h"
#include "events/electron_transfer.h"
#include "algorithms/vssm2_nodes.h"

//* Events update tree, with reactions as the leaf nodes
//
//           root
//          /    \
//     level 1 
//     /   \
//   level 2 
//    /   \
//  leaf 
// (event)
//
//*

namespace votca { namespace kmc {
 
//Class for the tree - functions that act on the whole tree (e.g. updating the tree)    
class Event_update_tree {

public:
     
    ~Event_update_tree() {};
    Event_update_tree() { Root = NULL; }; //initial empty tree   
    
    void initialise(State* _state, Graph* _graph);
        
    tree_node* root() { return Root; }
    void SetRoot(tree_node* _root) { root = _root; }
   
    void FindLeaf ( tree_node*, double rate );
    void path_from_leaf_to_root( tree_node* ); 
    bool dirtyflag () { return dirtyflag; }
    void update_tree();
    double cumulative_sum();
   
private:

    tree_node* Root;
    bool dirtyflag;
    
};

//Class for the nodes - functions that act only on one node at a time (e.g. adding a new child node)
class tree_node {
    
public:
    
    ~tree_node() {};
    tree_node() {Parent = NULL; child = NULL; }
    
    double Rate() { return rate; }
    void SetRate( double _rate ) { rate = _rate; }
    
    tree_node* parent() { return parent; }
    void SetParent(tree_node* _parent) { parent = _parent; }
    
    tree_node* leaf() {return leaf;}
    void SetLeaf(tree_node* _leaf) {leaf = _leaf; }
    
    void AddLeaf( tree_node* _leaf ) { 
       _leaf->SetParent( this );
       leaf.push_back( _leaf ); 
   };
   
    // iterator over child events
    typedef std::vector< tree_node* >::iterator iterator;
    typedef const std::vector< tree_node* >::iterator const_iterator;
    
    iterator begin() { return leaf.begin(); }
    iterator end() { return leaf.end(); }  
   
   //vector of leaf events
   void leaf_events(){
       
       for ( std::vector< tree_node* >::iterator it_leaf = leaf.begin(); it_leaf != leaf.end(); ++it_leaf ) {

       }
       
   }
   
    //tree_node* leftchild() { return leftchild; }
    //void SetLeftchild(tree_node* _leftchild) { leftchild = _leftchild; }
    
    //tree_node* rightchild() { return rightchild; }
    //void SetRightchild(tree_node* _rightchild) { rightchild = _rightchild; }
    
    //Flag the tree nodes that need to be newly enabled or disabled 
    void ToBeEnabled() { FlagEnabled = true; }
    bool FlagEnabled();
    void ToBeDisabled() { FlagDisabled = true; }
    bool FlagDisabled();
    
        
private:
    
    tree_node* Parent;
    std::vector < tree_node* > leaf;
    
    double rate;
      
};

void Event_update_tree::initialise(State* _state, Graph* _graph){
    
    State = _state;
    Graph = _graph;
    
    // Map of charge transfer events associated with a particular node
    std::unordered_map< BNode*, std::vector<Event*> > charge_transfer_map;
    
    //vector of electron transfer events
    std::vector<ElectronTransfer*> et_events;
   
    //Each node has a escape event
    for (Graph::iterator it_node = _graph->nodes_begin(); it_node != _graph->nodes_end(); ++it_node) {
        
        BNode* node_from = *it_node;
        
        
        // each escape event has an event move - looping over all edges
        for (BNode::EdgeIterator it_edge = node_from->EdgesBegin(); it_edge != node_from->EdgesEnd(); ++it_edge) {

 
        }
    }
   
    for (auto& event: et_events ) {
        BNode* node_from = event->NodeFrom();
        BNode* node_to = event->NodeTo();

        std::vector<Event*> events_to_disable = charge_transfer_map.at(node_from);
        std::vector<Event*> events_to_enable = charge_transfer_map.at(node_to);

        event->AddDisableOnExecute(&events_to_disable);
        event->AddEnableOnExecute(&events_to_enable);   
    }
    
    
    //tree_node->SetParent()
    
}

void Event_update_tree::FindLeaf(){
    
    
}

//Print the path from the root to the leaf node
void Event_update_tree::path_from_leaf_to_root(){
    
    
}

bool Event_update_tree::dirtyflag(){
    
    // If FlagEnabled() = true or FlagDisabled() = true
    //Use path_from_leaf_to_root to flag all of the affected nodes
    
    if (tree_node->FlagDisabled=true &| tree_node->FlagEnabled=true)
    {
        for (Event_update_tree->path_from_leaf_to_root()) {dirtyflag=true;}
    }  
    
}

double Event_update_tree::cumulative_sum(){

}

void Event_update_tree::update_tree(){
    
    //Every part of the tree with the dirty flag, has to be updated
    if (dirtyflag=true){
        
        //search for all nodes with dirty flag, move along the path to the root and update
        Event_update_tree->path_from_leaf_to_root();
        Event_update_tree->cumulative_sum();
    }
    
    
}

}} 


#endif
