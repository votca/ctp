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

#include <votca/kmc/event.h>
#include <votca/kmc/eventfactory.h>

namespace votca { namespace kmc {
        
    
class Event_update_tree {

public:
     
    ~Event_update_tree() {};
    Event_update_tree() { Root = NULL; }; //initial empty tree   
    
    void initialise(int leaf_size);
    double GetRates(Event* rate);
    void insert_rate(tree_node*, double new_rate);
    void cumulative_sum(tree_node*, double rate_sum);
    void check_tree();
    
    tree_node* root() { return Root; }
   
    void FindLeaf ( tree_node*, double rate );
    void path_from_leaf_to_root( tree_node* ); 
    bool dirtyflag () { return dirtyflag; }
    void update_tree();
   
private:
    
    tree_node* Root;
    
    //total number of nodes
    int tree_size;
    //height of the tree - for path to nodes
    double tree_height;
    //Should also have the number of leaf nodes = number of events
    int leaf_size;
    
    //after enabling or disabling a node - while traversing the hierarchy have to flag all tree nodes that now need updating
    //Tree is "dirty" - contains modifications which have not been committed to the current branch
    bool dirtyflag;
    
};

class tree_node {
    
public:    

    tree_node() {Parent = NULL; left = NULL; right = NULL; };
    ~tree_node() {};
   
    double rate() { return rate; }
    void new_rate(double _rate) { rate = _rate; }
    double rate_sum();
    
    tree_node* parent() { return parent; }
    void newParent(tree_node* _parent) { parent = _parent; }
    
    tree_node* leftchild() { return leftchild; }
    void newLeftchild(tree_node* _leftchild) { leftchild = _leftchild; }
    
    tree_node* rightchild() { return rightchild; }
    void newRightchild(tree_node* _rightchild) { rightchild = _rightchild; }
    
    //Flag the tree nodes that need to be newly enabled or disabled 
    void ToBeEnabled() { return FlagEnabled = true; }
    bool FlagEnabled();
    void ToBeDisabled() { return FlagDisabled = true; }
    bool FlagDisabled();
    
        
private:
    
    double rate; //Should be read from GetRate function
    tree_node* Parent;
    tree_node* leftchild;
    tree_node* rightchild;
    
      
};

void Event_update_tree::initialise(int leaf_size){
    
    leaf_size = sizeof(EventFactory->StoredEvents());
 
    //An initial tree size, to begin with, many tree nodes may be empty - can be filled with new events - must be full and complete
    //Assuming a perfect (full and balanced) tree - with l leaves, number of nodes n = 2l-1
    tree_size = (2*leaf_size)-1;
    //tree_height = (log2(tree_size+1));
    tree_height = ((log2(leaf_size))+1);
    
    //Create an empty tree, with tree_size number of nodes
    //tree_node* nodes = new tree_node(NULL);
}

void Event_update_tree::GetRates(Event* rate){
    
    //Function to read all of the rates for the events
    Event->Rate();
    //For each rate set as new_rate and move to insert
}

//Inserting a new node
void Event_update_tree::insert_rate(tree_node* newLeaf, double new_rate){
    
    tree_node* newLeaf = new tree_node();
    newLeaf->rate()=new_rate;
    
    //To set the newLeaf at the new left or right child
    //tree_node* _leftchild = (new_rate);
    //tree_node* _rightchild = (new_rate); 
    
    newLeaf->leftchild()=NULL;
    newLeaf->rightchild()=NULL;
}

void Event_update_tree::cumulative_sum(tree_node* leftchild, tree_node* rightchild, double rate_sum, double new_rate){
    
    //Creating a new parent as the sum of two leaf nodes
    if (tree_node->leftchild == NULL && tree_node->rightchild == NULL){
        
        tree_node* newParent = new tree_node();
        rate_sum = leftchild()+rightchild();
        newParent->rate()=rate_sum;
    }
    //Create a new parent until reaching the root node
    while ()
    {
        
    }
    
    //root node = left subtree + right subtree 
}

void Event_update_tree::check_tree( tree_node* ) {
    
    bool full_tree;
    //Check the tree is full and complete to continue, if not add empty nodes to balance
    if (((leaf_size)pow(1/tree_height))==2){
        std::cout >> "Full and complete tree" << std::endl; 
        full_tree = true;
        return;
    }
    else
    {
         //insert rates = 0 until "if" statement is true
        Event_update_tree->insert_rate(NULL);
        
    }  
    
}

void Event_update_tree::FindLeaf(tree_node* current, tree_node* choosen_leaf, double rate, double choosen_rate){
    
    //Start at root
    //If the chosen random number rand1 is < = root -> move left, else move right
    //continue down the levels of the tree until reaching leaf node
   
    for (current->leftchild()==NULL && current->rightchild()==NULL){
        if (choosen_rate = rate)
        {
            current = choosen_leaf;
            
            if (choosen_leaf->ToBeDisabled()){
                tree_node->FlagDisabled=true;
            }
            else if (choosen_leaf->ToBeEnabled()){
               tree_node->FlagEnabled=true; 
            }
            
        }
    //Once found flag as an enabled leaf or a disabled leaf with FlagEnabled() or FlagDisabled()

    }
    
}

//Print the path from the root to the leaf node
void Event_update_tree::path_from_leaf_to_root(tree_node* current){
    
    //while it is a leaf node
    while (current->leftchild()==NULL && current->rightchild()==NULL){
        current = current->parent();
        return current;
    }
    if (current->parent() != NULL)  
      {
          current = current->parent(); //Keep moving up, until reaching the root - (with no parent node)  
      }
    else 
      {
        current = root;
        return root;
      }
    
    
}

bool Event_update_tree::dirtyflag(){
    
    // If FlagEnabled() = true or FlagDisabled() = true
    //Use path_from_leaf_to_root to flag all of the affected nodes
    
    if (tree_node->FlagDisabled=true &| tree_node->FlagEnabled=true)
    {
        Event_update_tree->path_from_leaf_to_root();
        dirtyflag=true;
    }  
    
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
