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

#include "tfhe_gate_bootstrapping_structures.h"
#include "lwekey.h"
#include "lweparams.h"
#include "lwesamples.h"
#include "tlwe.h"
#include "tgsw.h"

TFheGateBootstrappingParameterSet::TFheGateBootstrappingParameterSet(const int32_t ks_t, const int32_t ks_basebit, const LweParams* const in_out_params, const TGswParams* const tgsw_params):
    ks_t(ks_t),
    ks_basebit(ks_basebit),
    in_out_params(in_out_params),
    tgsw_params(tgsw_params)
{}

TFheGateBootstrappingCloudKeySet::TFheGateBootstrappingCloudKeySet(
        const TFheGateBootstrappingParameterSet* const params,
        const LweBootstrappingKey* const bk,
        const LweBootstrappingKeyFFT* const bkFFT):
    params(params),bk(bk),bkFFT(bkFFT)
{}

TFheGateBootstrappingSecretKeySet::TFheGateBootstrappingSecretKeySet(
        const TFheGateBootstrappingParameterSet* const params,
        const LweBootstrappingKey* const bk,
        const LweBootstrappingKeyFFT* const bkFFT,
        const LweKey* lwe_key,
        const TGswKey* tgsw_key):
    params(params),
    lwe_key(lwe_key),
    tgsw_key(tgsw_key),
    cloud(params,bk,bkFFT)
{}
