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

#include <votca/kmc/kmccalculatorfactory.h>
#include "calculators/kmcmultiple.h"
#include "calculators/pbc.h"
#include "calculators/slab.h"
#include "calculators/excited.h"
//#include "calculators/kmcparallel.h"

namespace votca { namespace kmc {

void KMCCalculatorFactory::RegisterAll(void)
{
    Calculators().Register<KMCMultiple>("kmcmultiple"); // multiple charge carriers
    Calculators().Register<PBC>("pbc"); // precomputed rates for multiple carriers in PBC
    //Calculators().Register<Slab>("slab"); // multiple charge carriers with electrodes 
    //Calculators().Register<Excited>("excited"); // excited molecule energy transfer hops depend on distance (foster/dexter)   
    //Calculators().Register<Diffusion>("diffusion"); // single charge carrier in PBC
    //Calculators().Register<Diode>("diode"); // multiple charge types in 2D PBC
    //Calculators().Register<KMCParallel>("kmcparallel"); // single charge carrier threaded

}

}}
