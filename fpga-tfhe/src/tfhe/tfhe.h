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

#ifndef TFHE_H
#define TFHE_H

///@file
///@brief This file declares almost everything

#include "tfhe_core.h"

#include "numeric_functions.h"

#include "polynomials_arithmetic.h"
#include "lagrangehalfc_arithmetic.h"

#include "lwe-functions.h"

#include "tlwe_functions.h"

#include "tgsw_functions.h"

#include "lwekeyswitch.h"

#include "lwebootstrappingkey.h"

#include "tfhe_gate_bootstrapping_functions.h"

#include "tfhe_io.h"

///////////////////////////////////////////////////
//  TFHE bootstrapping internal functions
//////////////////////////////////////////////////


/** sets the seed of the random number generator to the given values */
EXPORT void tfhe_random_generator_setSeed(uint32_t* values, int32_t size);

EXPORT void tfhe_blindRotate(TLweSample* accum, const TGswSample* bk, const int32_t* bara, const int32_t n, const TGswParams* bk_params);
EXPORT void tfhe_blindRotateAndExtract(LweSample* result, const TorusPolynomial* v, const TGswSample* bk, const int32_t barb, const int32_t* bara, const int32_t n, const TGswParams* bk_params);
EXPORT void tfhe_bootstrap_woKS(LweSample* result, const LweBootstrappingKey* bk, Torus32 mu, const LweSample* x);
EXPORT void tfhe_bootstrap(LweSample* result, const LweBootstrappingKey* bk, Torus32 mu, const LweSample* x);
EXPORT void tfhe_createLweBootstrappingKey(LweBootstrappingKey* bk, const LweKey* key_in, const TGswKey* rgsw_key);

EXPORT void tfhe_blindRotate_FFT(TLweSample* accum, const TGswSampleFFT* bk, const int32_t* bara, const int32_t n, const TGswParams* bk_params);
EXPORT void tfhe_blindRotateAndExtract_FFT(LweSample* result, const TorusPolynomial* v, const TGswSampleFFT* bk, const int32_t barb, const int32_t* bara, const int32_t n, const TGswParams* bk_params);
EXPORT void tfhe_bootstrap_woKS_FFT(LweSample* result, const LweBootstrappingKeyFFT* bk, Torus32 mu, const LweSample* x);
EXPORT void tfhe_bootstrap_FFT(LweSample* result, const LweBootstrappingKeyFFT* bk, Torus32 mu, const LweSample* x);


#endif //TFHE_H
