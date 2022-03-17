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
#pragma HLS INTERFACE m_axi port = poly1 offset = slave bundle = gmem0
#pragma HLS INTERFACE m_axi port = poly2 offset = slave bundle = gmem1
#pragma HLS INTERFACE m_axi port = result offset = slave bundle = gmem0

#pragma HLS INTERFACE s_axilite port = poly1
#pragma HLS INTERFACE s_axilite port = poly2
#pragma HLS INTERFACE s_axilite port = result
#pragma HLS INTERFACE s_axilite port = return

	APCplx tmp0[PolyProcessor::N2];
	APCplx tmp1[PolyProcessor::N2];
	APCplx tmp2[PolyProcessor::N2];
	APTorus32 tmpT[PolyProcessor::N2];

	for (int i = 0; i < PolyProcessor::N2; i++)
	{
		tmp0[i] = 0;
		tmp1[i] = 0;
		tmp2[i] = 0;
	}

	proc[0] = PolyProcessor();

	executeReverseInt(proc, tmp0, poly1);
	executeReverseTorus32(proc, tmp1, poly2);
	lagrangeHalfCPolynomialMul(tmp2, tmp0, tmp1);
	executeDirectTorus32(proc, tmpT, tmp2);
	torusPolynomialAddTo(result, tmpT);
}
