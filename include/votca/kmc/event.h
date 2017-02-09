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

#include <votca/kmc/state.h>
#include <votca/tools/random2.h>

#ifndef __VOTCA_KMC_EVENT_H_
#define __VOTCA_KMC_EVENT_H_

namespace votca { namespace kmc {
  
class Event {
    
public:
    
   Event(){ enabled = false; expired = false; has_expired_subordinates = false; };
   virtual ~Event(){};     

   virtual std::string Type() = 0;
   virtual void OnExecute( State* state, votca::tools::Random2 *RandomVariable ) = 0;
          
   void Expire() { expired = true; parent->has_expired_subordinates = true; }  
   bool Expired(){ return expired; };
   
   // Disables the event (its rate is removed from the Cumulative Rate of the parent)
   void Disable(){ enabled = false; };
   // Enables the event (its rate is added to the Cumulative Rate of the parent)
   void Enable(){  enabled = true;  };
   bool Enabled(){ return enabled; };
  
   double Rate() { return rate; };
   void SetRate( double _rate ) { rate = _rate; };
   
   Event* GetParent(){ return parent; };
   void SetParent( Event* _parent ){ parent = _parent; };
   
   void AddSubordinate( Event* _event ) { 
       _event->SetParent( this );
       subordinate.push_back( _event ); 
   };
      
   // marks all subordinate events as expired
   void ExpireSubordinates(){
       for ( std::vector< Event* >::iterator it = subordinate.begin(); it != subordinate.end(); ++it ) {
           (*it)->Expire();
       }
       //std::cout << "Expired " << subordinate.size() << " subordinate events" << std::endl;
   }
   
   void RemoveExpired() {
       if ( has_expired_subordinates ) {
           Event* event;
            int count = 0;
           for ( std::vector< Event* >::iterator it = subordinate.begin(); it != subordinate.end(); ) {
               event = *it;
               if ( event->Expired() ) {
                   count++;
                   delete event;
                   subordinate.erase( it );
               } else {
                   ++it;
               }
           }
          //std::cout << "Removing expired events: " << count << " deleted" << std::endl;
           has_expired_subordinates = false;
       }
   }
   
   void RemoveSubordinate(Event* _event){
       //_event->SetParent( this );
       delete _event;
   }
   
   void ClearSubordinate() { subordinate.clear(); };
   
    // iterator over subordinate events
    typedef std::vector< Event* >::iterator iterator;
    typedef const std::vector< Event* >::iterator const_iterator;
    
    iterator begin() { return subordinate.begin(); }
    iterator end() { return subordinate.end(); }    

    // sum of all rates of enabled subordinate events
    double CumulativeRate() {

        if ( subordinate.empty() ) {return rate;}
        
        else
        {
            double rate = 0.0;
            for ( Event::iterator event = begin(); event != end(); ++event  ) {
                if ( (*event)->Enabled() ) { rate += (*event)->CumulativeRate(); }
            }
            return rate;
            }        
    }
   
    virtual void Print(std::string offset="") {
        std::cout << offset << Type(); 
                if (enabled) { std::cout << " enabled"; } else { std::cout << " disabled"; };
                std::cout << " subordinates: " << subordinate.size() 
                << " rate: " << rate 
                << " cumulative rate: " << CumulativeRate() <<  std::endl;
        offset += "-- "; 
        for(auto& event:  subordinate ) {
            event->Print(offset);
        }
    }

    
private:
    // if true, the rate of this event will be added to the Cumulative Rate of the parent node
    bool enabled;
    // if true, OnExecute of the parent node will remove this event after calling its OnExecute
    bool expired;
    // if the event has expired subordinates
    bool has_expired_subordinates;
    // Fixed rate in case the event does not have subordinate events
    double rate;
    
    // parent event
    Event* parent;
    
    // subordinate events
    std::vector< Event* > subordinate;    
};

}} 

#endif