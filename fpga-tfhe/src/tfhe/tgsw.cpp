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

#include "tfhe_core.h"
#include "tlwe.h"
#include "tgsw.h"
#include "polynomials_arithmetic.h"


TGswParams::TGswParams(int32_t l, int32_t Bgbit, const TLweParams *tlwe_params) :
        l(l),
        Bgbit(Bgbit),
        Bg(1 << Bgbit),
        halfBg(Bg / 2),
        maskMod(Bg - 1),
        tlwe_params(tlwe_params),
        kpl(int32_t((tlwe_params->k + 1) * l)) {
    h = new Torus32[l];
    for (int32_t i = 0; i < l; ++i) {
        int32_t kk = (32 - (i + 1) * Bgbit);
        h[i] = 1 << kk; // 1/(Bg^(i+1)) as a Torus32
    }

    // offset = Bg/2 * (2^(32-Bgbit) + 2^(32-2*Bgbit) + ... + 2^(32-l*Bgbit))
    uint32_t temp1 = 0;
    for (int32_t i = 0; i < l; ++i) {
        uint32_t temp0 = 1 << (32 - (i + 1) * Bgbit);
        temp1 += temp0;
    }
    offset = temp1 * halfBg;

}


TGswParams::~TGswParams() {
    delete[] h;
}


// same key as in TLwe
TGswKey::TGswKey(const TGswParams *params) :
        params(params), tlwe_params(params->tlwe_params), tlwe_key(tlwe_params) {
    key = tlwe_key.key;
}

TGswKey::~TGswKey() {
}


TGswSampleFFT::TGswSampleFFT(const TGswParams *params, TLweSampleFFT *all_samples_raw) : k(params->tlwe_params->k),
                                                                                         l(params->l) {
    all_samples = all_samples_raw;
    sample = new TLweSampleFFT *[(k + 1) * l];

    for (int32_t p = 0; p < (k + 1); ++p)
        sample[p] = all_samples + p * l;
}

TGswSampleFFT::~TGswSampleFFT() {
    delete[] sample;
}
