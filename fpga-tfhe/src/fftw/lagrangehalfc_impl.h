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

#ifndef LAGRANGEHALFC_IMPL_H
#define LAGRANGEHALFC_IMPL_H

#include <cassert>
#include <cmath>
#include <complex>
// typedef double _Complex cplx;
typedef std::complex< double > cplx; // https://stackoverflow.com/a/31800404
#include "tfhe/tfhe.h"
#include "tfhe/polynomials.h"

class FFT_Processor_nayuki {
public:
    const int32_t _2N;
    const int32_t N;
    const int32_t Ns2;

    double* real_inout;
    double* imag_inout;
    void* tables_direct;
    void* tables_reverse;

    FFT_Processor_nayuki(const int32_t N);
    void check_alternate_real();
    void check_conjugate_cplx();
    void execute_reverse_int(cplx* res, const int32_t* a);
    void execute_reverse_torus32(cplx* res, const Torus32* a);
    void execute_direct_torus32(Torus32* res, const cplx* a);
    ~FFT_Processor_nayuki();
};

extern thread_local FFT_Processor_nayuki fp1024_nayuki;

/**
 * structure that represents a real polynomial P mod X^N+1
 * as the N/2 complex numbers:
 * P(w), P(w^3), ..., P(w^(N-1))
 * where w is exp(i.pi/N)
 */
struct LagrangeHalfCPolynomial_IMPL
{
   cplx* coefsC;
   FFT_Processor_nayuki* proc;

   LagrangeHalfCPolynomial_IMPL(int32_t N);
   ~LagrangeHalfCPolynomial_IMPL();
};

#endif // LAGRANGEHALFC_IMPL_H
