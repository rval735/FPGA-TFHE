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
    constexpr static int32_t N2 = FFTTables::FFTSize;
    constexpr static int32_t N = FFTTables::FFTSize / 2;
    constexpr static int32_t Ns2 = FFTTables::FFTSize / 4;

    double realInOut[N2];
    double imagInOut[N2];

    FFTTables tablesForward = FFTTables(false);
    FFTTables tablesInverse = FFTTables(true);
};

void executeReverseInt(FFTProcessor *proc, cplx res[FFTProcessor::N], const int32_t a[FFTProcessor::N]);
void executeReverseTorus32(FFTProcessor *proc, cplx res[FFTProcessor::N], const Torus32 a[FFTProcessor::N]);
void executeDirectTorus32(FFTProcessor *proc, Torus32 res[FFTProcessor::N], const cplx a[FFTProcessor::N]);

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
