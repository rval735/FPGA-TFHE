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

#include "PolyKernel.hpp"

static PolyProcessor proc[1];

extern "C" void PolyKernel(const APInt32 poly1[PolyProcessor::N],
						   const APTorus32 poly2[PolyProcessor::N],
						   APTorus32 result[PolyProcessor::N])
{
	APCplx tmp0[PolyProcessor::N];
	APCplx tmp1[PolyProcessor::N];
	APCplx tmp2[PolyProcessor::N];
	APTorus32 tmpT[PolyProcessor::N];

	executeReverseInt(proc, tmp0, poly1);
	executeReverseTorus32(proc, tmp1, poly2);
	lagrangeHalfCPolynomialMul(tmp2, tmp0, tmp1);
	executeDirectTorus32(proc, tmpT, tmp2);
	torusPolynomialAddTo(result, tmpT);
}
