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

#ifndef TFHE_GARBAGE_COLLECTOR_H
#define TFHE_GARBAGE_COLLECTOR_H

///@file
///@brief This file declares a small garbage collector used for constant parameters

#ifndef __cplusplus
#error This file should only be included in a C++ file, for internal use only
#endif

#include <vector>
#include "tfhe_core.h"

/**
 * this class implements a simple garbage collector for simple parameter pointers
 */
class TfheGarbageCollector {
    std::vector<LweParams *> lwe_params;
    std::vector<TLweParams *> tlwe_params;
    std::vector<TGswParams *> tgsw_params;
    std::vector<TFheGateBootstrappingParameterSet *> gb_params;

    void m_register_param(LweParams *param);

    void m_register_param(TLweParams *param);

    void m_register_param(TGswParams *param);

    void m_register_param(TFheGateBootstrappingParameterSet *param);

    //TODO: parallelization
    static TfheGarbageCollector *global_tfheGarbageCollector;
public:
    /** initialize the garbage collector (this will automatically be
     * called by register_param */
    //TODO: parallelization
    static void init();

    /** adds a parameter to the list of managed parameters */
    //TODO: parallelization
    static void register_param(LweParams *param);

    /** adds a parameter to the list of managed parameters */
    //TODO: parallelization
    static void register_param(TLweParams *param);

    /** adds a parameter to the list of managed parameters */
    //TODO: parallelization
    static void register_param(TGswParams *param);

    /** adds a parameter to the list of managed parameters */
    //TODO: parallelization
    static void register_param(TFheGateBootstrappingParameterSet *param);

    /** destroy all managed parameters */
    //TODO: parallelization
    static void finalize();

    TfheGarbageCollector();

    TfheGarbageCollector(const TfheGarbageCollector &) = delete;

    void operator=(const TfheGarbageCollector &)= delete;

    ~TfheGarbageCollector();

};


#endif //TFHE_GARBAGE_COLLECTOR_H
