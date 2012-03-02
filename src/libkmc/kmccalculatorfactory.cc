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
//#include "votca_config.h"
#include "calculators/kmcsingle.h"
#include "calculators/diffusion.h"

namespace votca { namespace kmc {

void KMCCalculatorFactory::RegisterAll(void)
{
    Calculators().Register<KMCSingle>("kmcsingle"); // single charge carrier in PBC
    Calculators().Register<Diffusion>("diffusion"); // single charge carrier in PBC
}

}}
