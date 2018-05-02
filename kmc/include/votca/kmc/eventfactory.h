/*
 * Copyright 2009-2011 The VOTCA Development Team (http://www.votca.org)
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


#ifndef __VOTCA_KMC_EVENTFACTORY_H
#define	__VOTCA_KMC_EVENTFACTORY_H

#include <votca/tools/objectfactory.h>
#include "event.h"

namespace votca { namespace kmc {

using namespace tools;

class EventFactory
: public ObjectFactory< std::string, Event >
{
public:
    
    static void RegisterAll(void);

    friend EventFactory &Events();

    void AddEvent( Event* _event ) { 
        events.push_back(_event); 
        std::cout << "Adding event to the factory: " << _event->Type() << std::endl;
    };
    
    std::vector< Event* > StoredEvents(){return events; };  
    
private:
    
    std::vector< Event* > events;
    
    // private constructor - only one instance is allowed
    EventFactory() {}
    
    
};

inline EventFactory &Events()
{
    static EventFactory _instance;
    return _instance;
}

}}

#endif	/* __VOTCA_KMC_EVENTFACTORY_H */

