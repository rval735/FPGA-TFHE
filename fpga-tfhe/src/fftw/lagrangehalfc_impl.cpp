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

#include "tfhe/polynomials.h"
#include "lagrangehalfc_impl.h"

LagrangeHalfCPolynomial_IMPL::LagrangeHalfCPolynomial_IMPL(const int32_t N) {
    assert(N==1024);
    coefsC = new cplx[N/2];
    proc = &fp1024_nayuki;
}

LagrangeHalfCPolynomial_IMPL::~LagrangeHalfCPolynomial_IMPL() {
    delete[] coefsC;

}
//initialize the key structure
//(equivalent of the C++ constructor)
EXPORT void init_LagrangeHalfCPolynomial(LagrangeHalfCPolynomial* obj, const int32_t N) {
    new(obj) LagrangeHalfCPolynomial_IMPL(N);
}
EXPORT void init_LagrangeHalfCPolynomial_array(int32_t nbelts, LagrangeHalfCPolynomial* obj, const int32_t N) {
    for (int32_t i=0; i<nbelts; i++) {
	new(obj+i) LagrangeHalfCPolynomial_IMPL(N);
    }
}

//destroys the LagrangeHalfCPolynomial structure
//(equivalent of the C++ destructor)
EXPORT void destroy_LagrangeHalfCPolynomial(LagrangeHalfCPolynomial* obj) {
    LagrangeHalfCPolynomial_IMPL* objbis = (LagrangeHalfCPolynomial_IMPL*) obj;
    objbis->~LagrangeHalfCPolynomial_IMPL();
}
EXPORT void destroy_LagrangeHalfCPolynomial_array(int32_t nbelts, LagrangeHalfCPolynomial* obj) {
    LagrangeHalfCPolynomial_IMPL* objbis = (LagrangeHalfCPolynomial_IMPL*) obj;
    for (int32_t i=0; i<nbelts; i++) {
	(objbis+i)->~LagrangeHalfCPolynomial_IMPL();
    }
}


//MISC OPERATIONS
/** sets to zero */
EXPORT void LagrangeHalfCPolynomialClear(
	LagrangeHalfCPolynomial* reps) {
    LagrangeHalfCPolynomial_IMPL* reps1 = (LagrangeHalfCPolynomial_IMPL*) reps;
    const int32_t Ns2 = reps1->proc->Ns2;
    for (int32_t i=0; i<Ns2; i++)
	reps1->coefsC[i] = 0;
}

EXPORT void LagrangeHalfCPolynomialSetTorusConstant(LagrangeHalfCPolynomial* result, const Torus32 mu) {
    LagrangeHalfCPolynomial_IMPL* result1 = (LagrangeHalfCPolynomial_IMPL*) result;
    const int32_t Ns2 = result1->proc->Ns2;
    cplx* b = result1->coefsC;
    const cplx muc = t32tod(mu);
    for (int32_t j=0; j<Ns2; j++)
    	b[j]=muc;
}

EXPORT void LagrangeHalfCPolynomialAddTorusConstant(LagrangeHalfCPolynomial* result, const Torus32 mu) {
    LagrangeHalfCPolynomial_IMPL* result1 = (LagrangeHalfCPolynomial_IMPL*) result;
    const int32_t Ns2 = result1->proc->Ns2;
    cplx* b = result1->coefsC;
    const cplx muc = t32tod(mu);
    for (int32_t j=0; j<Ns2; j++)
    	b[j]+=muc;
}

/** termwise multiplication in Lagrange space */
EXPORT void LagrangeHalfCPolynomialMul(
	LagrangeHalfCPolynomial* result,
	const LagrangeHalfCPolynomial* a,
	const LagrangeHalfCPolynomial* b) {
    LagrangeHalfCPolynomial_IMPL* result1 = (LagrangeHalfCPolynomial_IMPL*) result;
    const int32_t Ns2 = result1->proc->Ns2;
    cplx* aa = ((LagrangeHalfCPolynomial_IMPL*) a)->coefsC;
    cplx* bb = ((LagrangeHalfCPolynomial_IMPL*) b)->coefsC;
    cplx* rr = result1->coefsC;
    for (int32_t i=0; i<Ns2; i++)
	rr[i] = aa[i]*bb[i];
}

/** termwise multiplication and addTo in Lagrange space */
EXPORT void LagrangeHalfCPolynomialAddMul(
	LagrangeHalfCPolynomial* accum,
	const LagrangeHalfCPolynomial* a,
	const LagrangeHalfCPolynomial* b)
{
    LagrangeHalfCPolynomial_IMPL* result1 = (LagrangeHalfCPolynomial_IMPL*) accum;
    const int32_t Ns2 = result1->proc->Ns2;
    cplx* aa = ((LagrangeHalfCPolynomial_IMPL*) a)->coefsC;
    cplx* bb = ((LagrangeHalfCPolynomial_IMPL*) b)->coefsC;
    cplx* rr = result1->coefsC;
    for (int32_t i=0; i<Ns2; i++)
	rr[i] += aa[i]*bb[i];
}


/** termwise multiplication and addTo in Lagrange space */
EXPORT void LagrangeHalfCPolynomialSubMul(
	LagrangeHalfCPolynomial* accum,
	const LagrangeHalfCPolynomial* a,
	const LagrangeHalfCPolynomial* b)
{
    LagrangeHalfCPolynomial_IMPL* result1 = (LagrangeHalfCPolynomial_IMPL*) accum;
    const int32_t Ns2 = result1->proc->Ns2;
    cplx* aa = ((LagrangeHalfCPolynomial_IMPL*) a)->coefsC;
    cplx* bb = ((LagrangeHalfCPolynomial_IMPL*) b)->coefsC;
    cplx* rr = result1->coefsC;
    for (int32_t i=0; i<Ns2; i++)
	rr[i] -= aa[i]*bb[i];
}

EXPORT void LagrangeHalfCPolynomialAddTo(
	LagrangeHalfCPolynomial* accum,
	const LagrangeHalfCPolynomial* a) {
    LagrangeHalfCPolynomial_IMPL* result1 = (LagrangeHalfCPolynomial_IMPL*) accum;
    const int32_t Ns2 = result1->proc->Ns2;
    cplx* aa = ((LagrangeHalfCPolynomial_IMPL*) a)->coefsC;
    cplx* rr = result1->coefsC;
    for (int32_t i=0; i<Ns2; i++)
	rr[i] += aa[i];
}
