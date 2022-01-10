// Project FPGA-TFHE
// https://github.com/rval735/FPGA-TFHE
//
// Original code from project
// https://github.com/tfhe/tfhe/
// Copyright 2016 - Nicolas Gama <nicolas.gama@gmail.com> et al.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "tfhe_garbage_collector.h"
#include "lwe-functions.h"
#include "tlwe_functions.h"
#include "tgsw_functions.h"
#include "tfhe_gate_bootstrapping_functions.h"

using namespace std;

/**
 * this class implements a simple garbage collector for simple parameter pointers
 */
void TfheGarbageCollector::m_register_param(LweParams* param) { lwe_params.push_back(param); }
void TfheGarbageCollector::m_register_param(TLweParams* param) { tlwe_params.push_back(param); }
void TfheGarbageCollector::m_register_param(TGswParams* param) { tgsw_params.push_back(param); }
void TfheGarbageCollector::m_register_param(TFheGateBootstrappingParameterSet* param) { gb_params.push_back(param); }

void TfheGarbageCollector::register_param(LweParams* param) { init(); global_tfheGarbageCollector->m_register_param(param); }

void TfheGarbageCollector::register_param(TLweParams* param) { init(); global_tfheGarbageCollector->m_register_param(param); }
void TfheGarbageCollector::register_param(TGswParams* param) { init(); global_tfheGarbageCollector->m_register_param(param); }
void TfheGarbageCollector::register_param(TFheGateBootstrappingParameterSet* param) { init(); global_tfheGarbageCollector->m_register_param(param); }

void TfheGarbageCollector::init() { if (!global_tfheGarbageCollector) global_tfheGarbageCollector = new TfheGarbageCollector(); }
void TfheGarbageCollector::finalize() { if (global_tfheGarbageCollector) delete global_tfheGarbageCollector; global_tfheGarbageCollector=0; }

TfheGarbageCollector::TfheGarbageCollector() {}
TfheGarbageCollector::~TfheGarbageCollector() {
    for (LweParams* param: lwe_params) delete_LweParams(param);
    for (TLweParams* param: tlwe_params) delete_TLweParams(param);
    for (TGswParams* param: tgsw_params) delete_TGswParams(param);
    for (TFheGateBootstrappingParameterSet* param: gb_params) delete_gate_bootstrapping_parameters(param);
    lwe_params.clear();
    tlwe_params.clear();
    tgsw_params.clear();
    gb_params.clear();
}

TfheGarbageCollector* TfheGarbageCollector::global_tfheGarbageCollector(0);
