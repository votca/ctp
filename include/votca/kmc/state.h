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

#ifndef __VOTCA_KMC_STATE_H_
#define __VOTCA_KMC_STATE_H_

#include <votca/kmc/carrier.h>
#include <votca/kmc/carrierfactory.h>

namespace votca { namespace kmc {
  
class State {
public:
    
    // Save and Load state into a file
    // can be used to resume the calculation
    void Save(std::string filename);
    void Load(std::string filename);
    
    Carrier* AddCarrier( std::string type );
    Event* Event (std::string Event_type);
    
    
private:

    std::vector< Carrier* > carriers;

};

void State::Save(std::string filename){
}

void State::Load(std::string filename){
}

// Add a carrier of a certain type
inline Carrier* State::AddCarrier( std::string type ) {

    cout << "Adding carrier " << type << endl;
    Carrier *carrier = Carriers().Create( type );
    carriers.push_back( carrier );
}

// Carrying out a specific event 
inline Event* State::Event(std::string Event_type) {
    cout << "Carrying out event " << Event_type << endl;
    Event *event = Events_Factory().Create(Event_type);
    Events_Factory.push_back( event );

}

}} 

#endif

