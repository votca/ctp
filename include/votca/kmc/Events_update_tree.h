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
     
    void initialise(int tree_size);
    
    //Event_update_tree() { Root = NULL; };
    ~Event_update_tree() {};
    
    tree_node* root() { return Root; }
    
    double GetRate(Event* rate);
    
    void insert(tree_node*, double rate);
    //void remove(tree_node*, double rate);
    
    //The various tree search methods
    void FindNode ( tree_node* );
    void print_levelorder( tree_node* );
    void printpathtoNode( tree_node* ); //Probably the most useful 
   
private:
    
    tree_node* Root;
    
    //Size of the tree is the total number of nodes
    int tree_size;
    //Should also have the number of leaf nodes = number of events
    int leaf_size;
    
    //after enabling or disabling a node - while traversing the hierarchy have to flag all tree nodes that now need updating
    bool dirtyflag();
    
};

class tree_node {
    
public:    

    tree_node() {Parent = NULL; left = NULL; right = NULL; };
    ~tree_node() {};
    
    double rate() { return rate; }
    void newrate(double _rate) { rate = _rate; }
    
    tree_node* parent() { return parent; }
    void newParent(tree_node* _parent) { parent = _parent; }
    
    tree_node* leftchild() { return leftchild; }
    void newLeftchild(tree_node* _leftchild) { leftchild = _leftchild; }
    
    tree_node* rightchild() { return rightchild; }
    void newRightchild(tree_node* _rightchild) { rightchild = _rightchild; }
    
        
private:
    
    double rate; //Should be read from GetRate function
    tree_node* Parent;
    tree_node* leftchild;
    tree_node* rightchild;
    
    //Flag the tree nodes that need to be newly enabled or disabled 
    bool FlagEnabled();
    bool FlagDisabled();
    
};

void Event_update_tree::initialise(int tree_size){
    //An initial tree size, to begin with, many tree nodes may be empty - can be filled with new events
}


//Inserting a new node - Root node
void Event_update_tree::insert(double data) {
    if ( Root == NULL ) 
    {
        tree_node* temp = new tree_node();
        temp->newData(data);
        Root = temp;
    }
    else {
        insert(Root, data);
    }
}

//Inserting a new node - child node
void Event_update_tree::insert(tree_node* current, double _rate) 
{
    if ( _rate <= current->rate() )  //data less than goes in the left subtree, the 'equal to' also has to be considered
    {
        if ( current->leftchild() != NULL)
        {
            insert(current->leftchild(), _rate); //while there is a left child, and new data is less than current keep moving left
        }
        else //When there is no child node, create a new node for the new data, with the current node as the parent
        {
            tree_node* temp = new tree_node();
            temp->newrate(_rate);
            temp->newParent(current);
            current->newLeftchild(temp);
        }
    }
    else 
    {
        if (current->rightchild() != NULL )
        {
            insert(current->rightchild(), _rate);
        }
        else 
        {
            tree_node* temp = new tree_node();
            temp->newrate(_rate);
            temp->newParent(current);
            current->newRightchild(temp);
        }
    }
}

//Print the tree in level order <root><level1><level2> ... 
void Event_update_tree::print_levelorder(tree_node* current);

//Print the path from the root to the leaf node
void Event_update_tree::printpathtoNode(tree_node* current);

}} 


#endif
