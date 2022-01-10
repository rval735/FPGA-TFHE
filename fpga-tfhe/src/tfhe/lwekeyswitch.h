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

#ifndef LWEKEYSWITCH_H
#define LWEKEYSWITCH_H

///@file
///@brief This file contains the declaration of LWE keyswitch structures

#include "tfhe_core.h"
#include "lweparams.h"
#include "lwesamples.h"

struct LweKeySwitchKey {
    int32_t n; ///< length of the input key: s'
    int32_t t; ///< decomposition length
    int32_t basebit; ///< log_2(base)
    int32_t base; ///< decomposition base: a power of 2
    const LweParams* out_params; ///< params of the output key s
    LweSample* ks0_raw; //tableau qui contient tout les Lwe samples de taille nlbase
    LweSample** ks1_raw;// de taille nl  pointe vers un tableau ks0_raw dont les cases sont espaceés de base positions
    LweSample*** ks; ///< the keyswitch elements: a n.l.base matrix
    // de taille n pointe vers ks1 un tableau dont les cases sont espaceés de ell positions

#ifdef __cplusplus
    LweKeySwitchKey(int32_t n, int32_t t, int32_t basebit, const LweParams* out_params, LweSample* ks0_raw);
    ~LweKeySwitchKey();
    LweKeySwitchKey(const LweKeySwitchKey&) = delete;
    void operator=(const LweKeySwitchKey&) = delete;
#endif
};

//allocate memory space for a LweKeySwitchKey
EXPORT LweKeySwitchKey* alloc_LweKeySwitchKey();
EXPORT LweKeySwitchKey* alloc_LweKeySwitchKey_array(int32_t nbelts);

//free memory space for a LweKeySwitchKey
EXPORT void free_LweKeySwitchKey(LweKeySwitchKey* ptr);
EXPORT void free_LweKeySwitchKey_array(int32_t nbelts, LweKeySwitchKey* ptr);

//initialize the LweKeySwitchKey structure
//(equivalent of the C++ constructor)
EXPORT void init_LweKeySwitchKey(LweKeySwitchKey* obj, int32_t n, int32_t t, int32_t basebit, const LweParams* out_params);
EXPORT void init_LweKeySwitchKey_array(int32_t nbelts, LweKeySwitchKey* obj, int32_t n, int32_t t, int32_t basebit, const LweParams* out_params);

//destroys the LweKeySwitchKey structure
//(equivalent of the C++ destructor)
EXPORT void destroy_LweKeySwitchKey(LweKeySwitchKey* obj);
EXPORT void destroy_LweKeySwitchKey_array(int32_t nbelts, LweKeySwitchKey* obj);

//allocates and initialize the LweKeySwitchKey structure
//(equivalent of the C++ new)
EXPORT LweKeySwitchKey* new_LweKeySwitchKey(int32_t n, int32_t t, int32_t basebit, const LweParams* out_params);
EXPORT LweKeySwitchKey* new_LweKeySwitchKey_array(int32_t nbelts, int32_t n, int32_t t, int32_t basebit, const LweParams* out_params);

//destroys and frees the LweKeySwitchKey structure
//(equivalent of the C++ delete)
EXPORT void delete_LweKeySwitchKey(LweKeySwitchKey* obj);
EXPORT void delete_LweKeySwitchKey_array(int32_t nbelts, LweKeySwitchKey* obj);

#endif // LWEKEYSWITCH_H
