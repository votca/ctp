 /*
 * Copyright 2009-2018 The VOTCA Development Team (http://www.votca.org)
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

#ifndef __VOTCA_KMC_MULTIPLE_H
#define	__VOTCA_KMC_MULTIPLE_H

#include <votca/tools/vec.h>
#include <votca/tools/random2.h>
#include <votca/kmc/kmccalculator.h>


namespace votca { namespace kmc {
    
// forward declaration used only in this calculator and implemented in the .cc file
class Node;

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
            votca::tools::vec _field;
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
