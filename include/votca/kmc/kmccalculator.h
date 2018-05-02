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

#ifndef __VOTCA_KMC_CALCULATOR_H
#define	__VOTCA_KMC_CALCULATOR_H

#include <votca/tools/calculator.h>
#include <votca/tools/property.h>

namespace votca { namespace kmc {

    using namespace tools;

class KMCCalculator : public Calculator
{
public:
    
    KMCCalculator() {};
    virtual     ~KMCCalculator() {};

    // identifies a calculator (needed for the filename of the xml file with help)
    virtual std::string  Identify() = 0;    
    // dummy implementation of the pure virtual function in tools
    void Initialize(Property *options) = 0;
    // need to check why this is needed
    virtual void Initialize(const char *filename, Property *options, const char *outputfile) {}
    // evaluate function every calculator runs
    virtual bool EvaluateFrame() { return true; }
    // data accumulation at the end of the run
    virtual void EndEvaluate() {}    
    // default values 
    void UpdateWithDefaults( votca::tools::Property *options ) {
         votca::tools::Calculator::UpdateWithDefaults(options, "kmc");
    }
    
    void         setnThreads(int nThreads) { _nThreads = nThreads; }
    
protected:
    
    int _nThreads;
};

}}

#endif	/* __VOTCA_KMC_CALCULATOR_H */

