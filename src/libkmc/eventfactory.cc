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

#include <votca/kmc/eventfactory.h>
#include "events/electron_transfer.h"
#include "events/electron_transfer_snail.h"
#include "events/electron_transfer_terminal.h"
#include "events/carrier_escape.h"
#include "events/e_injection.h"
#include "events/e_collection.h"

namespace votca { namespace kmc {

void EventFactory::RegisterAll(void)
{
    Events().Register<ElectronTransfer>("electron_transfer");
    Events().Register<ElectronTransferSnail>("electron_transfer_snail");
    Events().Register<CarrierEscape>("carrier_escape");
    Events().Register<ElectronTransferTerminal>("electron_transfer_terminal");
    Events().Register<ElectronInjection>("electron_injection");
    Events().Register<ElectronCollection>("electron_collection");
}

}}
