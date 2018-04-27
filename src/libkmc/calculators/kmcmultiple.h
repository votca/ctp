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

#ifndef __VOTCA_KMC_MULTIPLE_H
#define	__VOTCA_KMC_MULTIPLE_H

#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <votca/tools/vec.h>
#include <votca/tools/matrix.h>
#include <votca/tools/statement.h>
#include <votca/tools/database.h>
#include <votca/tools/tokenizer.h>
#include <votca/tools/globals.h>
#include <votca/tools/random2.h>
#include <votca/kmc/kmccalculator.h>

#include <unordered_map>
#include <cmath> // needed for abs(double)
//#include <votca/kmc/node.h>
#include <math.h> // needed for fmod()

//using namespace std;

typedef votca::tools::vec myvec;

namespace votca { namespace kmc {
    


const double kB = votca::tools::globals::constants::kB;
const double hbar = votca::tools::globals::constants::hbar;


static double Pi=votca::tools::globals::constants::pi;

typedef std::unordered_map<unsigned long, double> CoulombMap;
typedef CoulombMap::const_iterator CoulombIt;

class Node;
class Chargecarrier;


/**
* \brief Kinetic Monte Carlo with exclusion principle
*
* Evaluates of electron/hole mobilities for one carrier type  
* Requires state.sql and options.xml file.
* 
* 
* Executable: kmc_run
*/

class KMCMultiple : public KMCCalculator 
{
public:
    KMCMultiple() {};
   ~KMCMultiple() {};

    std::string  Identify() {return "kmcmultiple"; };
    
    void Initialize(Property *options) {}  
        
    void Initialize(const char *filename, Property *options, const char *outputfile );
    bool EvaluateFrame();

protected:
	    vector<Node*>  LoadGraph();

            vector<double> RunVSSM(vector<Node*> node, double runtime, unsigned int numberofcharges, votca::tools::Random2 *RandomVariable);
            void WriteOcc(vector<double> occP, vector<Node*> node);
            void InitialRates(vector<Node*> node);

            void InitBoxSize(vector<Node*> node);

            string _injection_name;
            string _injectionmethod;
            int _injectionfree;
            double _runtime;
            double _maxrealtime;
            int _seed;
            int _numberofcharges;
            int _allowparallel;
            double _fieldX;
            double _fieldY;
            double _fieldZ;
            myvec _field;
            double _outputtime;
            string _trajectoryfile;
            string _timefile;
            string _carriertype;
            double _temperature;
            string _filename;
            string _outputfile;
            double _q;
            double _boxsizeX;
            double _boxsizeY;
            double _boxsizeZ;
            string _rates;
            int    _tof;
            double _toflength;
            string _tofdirection;
            double _coulomboffset;
};


}}


#endif	/* __VOTCA_KMC_MULTIPLE_H */
