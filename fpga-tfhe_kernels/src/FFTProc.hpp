// Project FPGA-TFHE
// https://github.com/rval735/FPGA-TFHE
//
// Based on code from project
// https://github.com/tfhe/tfhe/
//
//                    GNU GENERAL PUBLIC LICENSE
//                       Version 3, 29 June 2007
//
// Copyright (C) 2007 Free Software Foundation, Inc. <https://fsf.org/>
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.


#ifndef FFTPROC_HPP
#define FFTPROC_HPP

#include <cassert>
#include <cmath>
#include <complex>

#include "FFTTables.hpp"

// typedef double _Complex cplx;
typedef std::complex<double> cplx; // https://stackoverflow.com/a/31800404
typedef int32_t Torus32; //avant uint32_t
//#include "tfhe/tfhe.h"
//#include "tfhe/polynomials.h"

class FFTProcessor
{
public:
    constexpr static int32_t _2N = FFTTables::FFTSize;
    constexpr static int32_t N = FFTTables::FFTSize / 2;
    constexpr static int32_t Ns2 = FFTTables::FFTSize / 4;

    double realInOut[_2N];
    double imagInOut[_2N];
    cplx omegaxminus1[_2N];
    FFTTables tablesForward = FFTTables(false);
    FFTTables tablesInverse = FFTTables(true);

    FFTProcessor();
    void executeReverseInt(cplx *res, const int32_t *a);
    void executeReverseTorus32(cplx *res, const Torus32 *a);
    void executeDirectTorus32(Torus32 *res, const cplx *a);
};

////extern thread_local FFT_Processor_nayuki fp1024_nayuki;
//
///**
// * structure that represents a real polynomial P mod X^N+1
// * as the N/2 complex numbers:
// * P(w), P(w^3), ..., P(w^(N-1))
// * where w is exp(i.pi/N)
// */
//struct LagrangeHalfCPolynomial_IMPL
//{
//   cplx* coefsC;
//   FFT_Processor_nayuki* proc;
//
//   LagrangeHalfCPolynomial_IMPL(int32_t N);
//   ~LagrangeHalfCPolynomial_IMPL();
//};

#endif // FFTPROC_HPP
