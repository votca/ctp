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

#ifndef __VOTCA_KMC_TERMINAL_ALGORITHM_H_
#define __VOTCA_KMC_TERMINAL_ALGORITHM_H_

#include <votca/kmc/event.h>

namespace votca { namespace kmc {
  
class TerminalAlgorithm {
public:
    
    TerminalAlgorithm(){};
    
    virtual void Initialize(State* _state, TerminalGraph* _graph) = 0;
 
    double Time(){ return time; }
    
    virtual void Run( double runtime, int nsteps, int seed, int nelectrons, int nholes, string trajectoryfile, double outtime, double fieldX, double fieldY, double fieldZ) = 0;
       
    virtual void progressbar(double fraction) = 0;
    
protected:
   
    double time;
    
    State* state;
    
    TerminalGraph* terminalgraph;
};

    
}}



#endif

