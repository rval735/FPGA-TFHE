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

#include "FFTL2Kernel.hpp"

static FFTProcessor proc[1];

extern "C" void FFTL2Kernel(const APInt32 poly1[FFTProcessor::N],
							const APTorus32 poly2[FFTProcessor::N],
							APTorus32 result[FFTProcessor::N])
{
	APCplx tmp0[FFTProcessor::N];
	APCplx tmp1[FFTProcessor::N];
	APCplx tmp2[FFTProcessor::N];
	APTorus32 tmpT[FFTProcessor::N];

	executeReverseInt(proc, tmp0, poly1);
	executeReverseTorus32(proc, tmp1, poly2);
	lagrangeHalfCPolynomialMul(tmp2, tmp0, tmp1);
	executeDirectTorus32(proc, tmpT, tmp2);
	torusPolynomialAddTo(result, tmpT);
}
