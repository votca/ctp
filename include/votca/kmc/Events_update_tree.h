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
    Event_update_tree() { Root = NULL; };
    ~Event_update_tree() {};
    
    tree_node* root() { return Root; }
    
    void insert(tree_node*, double);
    void remove(tree_node*, double);
    
    //The various tree search methods to choose from
    void print_levelorder( tree_node* );
    void print_preorder( tree_node* );
    void print_inorder( tree_node* );
    void print_postorder( tree_node* );
    void printpathtoNode( tree_node* ); //Probably the most useful 
   
private:
    tree_node* Root;
};

class tree_node {
    
public:    

    tree_node() {Parent = NULL; left = NULL; right = NULL; };
    ~tree_node() {};
    
    double data() { return Data; }
    void newData(double data) { Data = data; }
    
    tree_node* parent() { return Parent; }
    void newParent(tree_node* parent) { Parent = parent; }
    
    tree_node* leftchild() { return left; }
    void newLeftchild(tree_node* leftchild) { left = leftchild; }
    
    tree_node* rightchild() { return right; }
    void newRightchild(tree_node* rightchild) { right = rightchild; }
    
        
private:
    
    double Data; //The information contained in each node - type of data can be changed
    tree_node* Parent;
    tree_node* left;
    tree_node* right;
    
    
};

//Binary Tree Forest implementation: Grouping individual tree structures together, linked together via a vector of pointers or nodes
//Vector points to each of the trees roots - minimising each search to one specific tree

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
void Event_update_tree::insert(tree_node* current, double new_data) 
{
    if ( new_data <= current->data() )  //data less than goes in the left subtree, the 'equal to' also has to be considered
    {
        if ( current->leftchild() != NULL)
        {
            insert(current->leftchild(), new_data); //while there is a left child, and new data is less than current keep moving left
        }
        else //When there is no child node, create a new node for the new data, with the current node as the parent
        {
            tree_node* temp = new tree_node();
            temp->newData(new_data);
            temp->newParent(current);
            current->newLeftchild(temp);
        }
    }
    else 
    {
        if (current->rightchild() != NULL )
        {
            insert(current->rightchild(), new_data);
        }
        else 
        {
            tree_node* temp = new tree_node();
            temp->newData(new_data);
            temp->newParent(current);
            current->newRightchild(temp);
        }
    }
}

//Print the tree in level order <root><level1><level2> ... 
void Event_update_tree::print_levelorder(tree_node* current);

//Print the tree in pre-order <root><leftsubtree><rightsubtree>
void Event_update_tree::print_preorder(tree_node* current);

//Print the tree in-order <leftsubtree><root><rightsubtree>
void Event_update_tree::print_inorder(tree_node* current);

//Print the tree in post-order <leftsubtree> <rightsubtree> <root>
void Event_update_tree::print_postorder(tree_node* current);

//Print the path from the root to the leaf node
void Event_update_tree::printpathtoNode(tree_node* current);

}} 


#endif
