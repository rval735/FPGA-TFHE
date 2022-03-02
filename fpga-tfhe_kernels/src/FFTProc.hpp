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

#include <hls_x_complex.h>

#include "FFTTables.hpp"

//https://support.xilinx.com/s/question/0D52E00006mhdOTSAY/whats-better-stdcomplex-or-hlsxcomplex?language=en_US
// Every variable needs to add "__attribute((no_ctor))"
typedef hls::x_complex<APDouble> APCplx;

class FFTProcessor
{
public:
    constexpr static int N2 = FFTTables::FFTSize;
    constexpr static int N = FFTTables::FFTSize / 2;
    constexpr static int Ns2 = FFTTables::FFTSize / 4;

    APDouble realInOut[N2];
    APDouble imagInOut[N2];

    FFTProcessor();
};

void executeReverseInt(FFTProcessor proc[1],
					   APCplx res[FFTProcessor::N],
					   const APInt32 a[FFTProcessor::N]);

void executeReverseTorus32(FFTProcessor proc[1],
						   APCplx res[FFTProcessor::N],
						   const APTorus32 a[FFTProcessor::N]);

void executeDirectTorus32(FFTProcessor proc[1],
						  APTorus32 res[FFTProcessor::N],
						  const APCplx a[FFTProcessor::N]);

void lagrangeHalfCPolynomialMul(APCplx result[FFTProcessor::Ns2],
								APCplx a[FFTProcessor::Ns2],
								APCplx b[FFTProcessor::Ns2]);

void torusPolynomialAddTo(APTorus32 result[FFTProcessor::N],
						  const APTorus32 b[FFTProcessor::N]);

#endif // FFTPROC_HPP
