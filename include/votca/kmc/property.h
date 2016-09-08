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

#ifndef __VOTCA_KMC_PROPERTY_H_
#define __VOTCA_KMC_PROPERTY_H_

#include <votca/tools/property.h>

namespace votca { namespace kmc {
class Property {
    
public: 
    
    Property _job_summary;
    
    //Property &add(const string &key, const string &value);
    Property *_output_summary = &_job_summary.add("output","");
    Property *_segment_summary = &_output_summary->add("segment","");
    
    //Property &get(const string &key);
    string segName = seg->getName();
    segId = seg->getId();
    
    //Property &set(const string &key, const string &value);
    _segment_summary-> setAttribute("id", segId);
    _segment_summary-> setAttribute("type", segName);
    _segment_summary-> setAttribute("homo", _orbitals.getEnergy(_orbitals.getNumberOfElectrons() ));
    _segment_summary-> setAttribute("lumo", _orbitals.getEnergy(_orbitals.getNumberOfElectrons() + 1 ));

    
private:

};
 
}}

#endif


