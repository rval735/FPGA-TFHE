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

#ifndef LweBOOTSTRAPPINGKEY_H
#define LweBOOTSTRAPPINGKEY_H

///@file
///@brief This file contains the declaration of bootstrapping key structures

#include "tfhe_core.h"


struct LweBootstrappingKey{
    const LweParams* in_out_params; ///< paramètre de l'input et de l'output. key: s
    const TGswParams* bk_params; ///< params of the Gsw elems in bk. key: s"
    const TLweParams* accum_params; ///< params of the accum variable key: s"
    const LweParams* extract_params; ///< params after extraction: key: s'
    TGswSample* bk; ///< the bootstrapping key (s->s")
    LweKeySwitchKey* ks; ///< the keyswitch key (s'->s)


#ifdef __cplusplus
   LweBootstrappingKey(const LweParams* in_out_params,
    const TGswParams* bk_params,
    const TLweParams* accum_params,
    const LweParams* extract_params,
    TGswSample* bk,
    LweKeySwitchKey* ks);
    ~LweBootstrappingKey();
    LweBootstrappingKey(const LweBootstrappingKey&) = delete;
    void operator=(const LweBootstrappingKey&) = delete;

#endif


};


struct LweBootstrappingKeyFFT {
    const LweParams* in_out_params; ///< paramètre de l'input et de l'output. key: s
    const TGswParams* bk_params; ///< params of the Gsw elems in bk. key: s"
    const TLweParams* accum_params; ///< params of the accum variable key: s"
    const LweParams* extract_params; ///< params after extraction: key: s'
    const TGswSampleFFT* bkFFT; ///< the bootstrapping key (s->s")
    const LweKeySwitchKey* ks; ///< the keyswitch key (s'->s)


#ifdef __cplusplus
   LweBootstrappingKeyFFT(const LweParams* in_out_params,
    const TGswParams* bk_params,
    const TLweParams* accum_params,
    const LweParams* extract_params,
    const TGswSampleFFT* bkFFT,
    const LweKeySwitchKey* ks);
    ~LweBootstrappingKeyFFT();
    LweBootstrappingKeyFFT(const LweBootstrappingKeyFFT&) = delete;
    void operator=(const LweBootstrappingKeyFFT&) = delete;

#endif


};


//allocate memory space for a LweBootstrappingKey
EXPORT LweBootstrappingKey* alloc_LweBootstrappingKey();
EXPORT LweBootstrappingKey* alloc_LweBootstrappingKey_array(int32_t nbelts);

//free memory space for a LweBootstrappingKey
EXPORT void free_LweBootstrappingKey(LweBootstrappingKey* ptr);
EXPORT void free_LweBootstrappingKey_array(int32_t nbelts, LweBootstrappingKey* ptr);

//initialize the LweBootstrappingKey structure
//(equivalent of the C++ constructor)
EXPORT void init_LweBootstrappingKey(LweBootstrappingKey* obj, int32_t ks_t, int32_t ks_basebit, const LweParams* in_out_params, const TGswParams* bk_params);
EXPORT void init_LweBootstrappingKey_array(int32_t nbelts, LweBootstrappingKey* obj, int32_t ks_t, int32_t ks_basebit, const LweParams* in_out_params, const TGswParams* bk_params);

//destroys the LweBootstrappingKey structure
//(equivalent of the C++ destructor)
EXPORT void destroy_LweBootstrappingKey(LweBootstrappingKey* obj);
EXPORT void destroy_LweBootstrappingKey_array(int32_t nbelts, LweBootstrappingKey* obj);

//allocates and initialize the LweBootstrappingKey structure
//(equivalent of the C++ new)
EXPORT LweBootstrappingKey* new_LweBootstrappingKey(const int32_t ks_t, const int32_t ks_basebit, const LweParams* in_out_params, const TGswParams* bk_params);
EXPORT LweBootstrappingKey* new_LweBootstrappingKey_array(int32_t nbelts, const int32_t ks_t, const int32_t ks_basebit, const LweParams* in_out_params, const TGswParams* bk_params);

//destroys and frees the LweBootstrappingKey structure
//(equivalent of the C++ delete)
EXPORT void delete_LweBootstrappingKey(LweBootstrappingKey* obj);
EXPORT void delete_LweBootstrappingKey_array(int32_t nbelts, LweBootstrappingKey* obj);

//allocate memory space for a LweBootstrappingKeyFFT
EXPORT LweBootstrappingKeyFFT* alloc_LweBootstrappingKeyFFT();
EXPORT LweBootstrappingKeyFFT* alloc_LweBootstrappingKeyFFT_array(int32_t nbelts);

//free memory space for a LweBootstrappingKeyFFT
EXPORT void free_LweBootstrappingKeyFFT(LweBootstrappingKeyFFT* ptr);
EXPORT void free_LweBootstrappingKeyFFT_array(int32_t nbelts, LweBootstrappingKeyFFT* ptr);

//initialize the LweBootstrappingKeyFFT structure
//(equivalent of the C++ constructor)
EXPORT void init_LweBootstrappingKeyFFT(LweBootstrappingKeyFFT* obj, const LweBootstrappingKey* bk);
EXPORT void init_LweBootstrappingKeyFFT_array(int32_t nbelts, LweBootstrappingKeyFFT* obj, const LweBootstrappingKey* bk);

//destroys the LweBootstrappingKeyFFT structure
//(equivalent of the C++ destructor)
EXPORT void destroy_LweBootstrappingKeyFFT(LweBootstrappingKeyFFT* obj);
EXPORT void destroy_LweBootstrappingKeyFFT_array(int32_t nbelts, LweBootstrappingKeyFFT* obj);

//allocates and initialize the LweBootstrappingKeyFFT structure
//(equivalent of the C++ new)
EXPORT LweBootstrappingKeyFFT* new_LweBootstrappingKeyFFT(const LweBootstrappingKey* bk);
EXPORT LweBootstrappingKeyFFT* new_LweBootstrappingKeyFFT_array(int32_t nbelts, const LweBootstrappingKey* bk);

//destroys and frees the LweBootstrappingKeyFFT structure
//(equivalent of the C++ delete)
EXPORT void delete_LweBootstrappingKeyFFT(LweBootstrappingKeyFFT* obj);
EXPORT void delete_LweBootstrappingKeyFFT_array(int32_t nbelts, LweBootstrappingKeyFFT* obj);

#endif
