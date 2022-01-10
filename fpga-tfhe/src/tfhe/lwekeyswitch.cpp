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

#include "lwekeyswitch.h"

LweKeySwitchKey::LweKeySwitchKey(int32_t n, int32_t t, int32_t basebit, const LweParams* out_params, LweSample* ks0_raw){
    this->basebit=basebit;
    this->out_params=out_params;
    this->n=n;
    this->t=t;
    this->base=1<<basebit;
    this->ks0_raw = ks0_raw;
    ks1_raw = new LweSample*[n*t];
    ks = new LweSample**[n];


    for (int32_t p = 0; p < n*t; ++p)
	    ks1_raw[p] = ks0_raw + base*p;
	for (int32_t p = 0; p < n; ++p)
	    ks[p] = ks1_raw + t*p;
}

LweKeySwitchKey::~LweKeySwitchKey() {
    delete[] ks1_raw;
    delete[] ks;
}
