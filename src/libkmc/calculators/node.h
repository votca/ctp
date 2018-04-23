/* 
 *
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
 * Pascal Kordt and Timo Pulch
 */

#ifndef NODE_H
#define	NODE_H
#include <votca/tools/vec.h>

using namespace votca::kmc;


struct Event
{
    int destination;
    double rate;
    votca::tools::vec dr;
    
    // stuff for Coulomb interaction
    double Jeff2;
    double reorg_out;
    double initialrate;
};


class Node
{
    public:
        int id;
        int occupied;
        int injectable;
        double escaperate;
        double initialescaperate;
        double occupationtime;
        votca::tools::vec position;
        vector<Event> event;
        vector<Event> forbiddenevent;
        // stuff for Coulomb interaction:
        double siteenergy;
        double reorg_intorig; // UnCnN
        double reorg_intdest; // UcNcC
    
        double EscapeRate();
        
        void AddEvent(int seg2, double rate12, votca::tools::vec dr, double Jeff2, double reorg_out);
        void InitEscapeRate();
        void AddForbiddenEvent(int seg2, double rate12);
        void ClearForbiddenevents();
        void RemoveForbiddenEvent(int seg2);
};


void Node::AddEvent(int seg2, double rate12, votca::tools::vec dr, double Jeff2, double reorg_out)
{
    Event newEvent;
    newEvent.destination = seg2;
    newEvent.rate = rate12;
    newEvent.initialrate = rate12;
    newEvent.dr = dr;
    newEvent.Jeff2 = Jeff2;
    newEvent.reorg_out = reorg_out;
    this->event.push_back(newEvent);
};

void Node::AddForbiddenEvent(int seg2, double rate12)
{
    Event newEvent;
    newEvent.destination = seg2;
    newEvent.rate = rate12;
    newEvent.initialrate = rate12;
    newEvent.dr = {0,0,0};
    newEvent.Jeff2 = 0;
    newEvent.reorg_out = 0;
    this->forbiddenevent.push_back(newEvent);
};


void Node::InitEscapeRate()
{
    double newEscapeRate = 0;
    for(unsigned int i=0; i<this->event.size();i++)
    {
        newEscapeRate += this->event[i].rate;
    }
    this->escaperate = newEscapeRate;
    //EDITED
    this->initialescaperate = newEscapeRate;
    // cout << "Escape rate for segment " << this->id << " was set to " << newEscapeRate << endl;
};
void Node::RemoveForbiddenEvent(int seg2)
{
    for(unsigned int i=0; i<this->forbiddenevent.size();i++){
        if (this->forbiddenevent[i].destination ==seg2){
            this->escaperate = this->escaperate+this->forbiddenevent[i].rate;
            this->forbiddenevent.erase(forbiddenevent.begin()+i);
        }
    }
    
};

void Node::ClearForbiddenevents()
{
    this->forbiddenevent = {};
}

double Node::EscapeRate()
{
    return escaperate;
};

#endif	/* NODE_H */

