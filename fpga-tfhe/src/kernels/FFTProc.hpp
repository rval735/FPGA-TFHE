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

//#include <complex>
#include <hls_x_complex.h>

#include "FFTTables.hpp"

// typedef double _Complex cplx;
//typedef std::complex<double> cplx; // https://stackoverflow.com/a/31800404
typedef APInt32 APTorus32;
//#include "tfhe/tfhe.h"
//#include "tfhe/polynomials.h"

//https://support.xilinx.com/s/question/0D52E00006mhdOTSAY/whats-better-stdcomplex-or-hlsxcomplex?language=en_US
// Every variable needs to add "__attribute((no_ctor))"
//typedef std::complex<APDouble> APCplx;
typedef hls::x_complex<APDouble> APCplx;

class FFTProcessor
{
public:
    constexpr static int N2 = FFTTables::FFTSize;
    constexpr static int N = FFTTables::FFTSize / 2;
    constexpr static int Ns2 = FFTTables::FFTSize / 4;

    APDouble realInOut[N2];
    APDouble imagInOut[N2];
};

void executeReverseInt(FFTProcessor proc[1], APCplx res[FFTProcessor::N], const APInt32 a[FFTProcessor::N]);
void executeReverseTorus32(FFTProcessor proc[1], APCplx res[FFTProcessor::N], const APTorus32 a[FFTProcessor::N]);
void executeDirectTorus32(FFTProcessor proc[1], APTorus32 res[FFTProcessor::N], const APCplx a[FFTProcessor::N]);
void lagrangeHalfCPolynomialMul(APCplx result[FFTProcessor::Ns2],
								APCplx a[FFTProcessor::Ns2],
								APCplx b[FFTProcessor::Ns2]);
void torusPolynomialAddTo(APTorus32 result[FFTProcessor::N], const APTorus32 b[FFTProcessor::N]);

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
