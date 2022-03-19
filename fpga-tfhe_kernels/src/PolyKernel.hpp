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

#ifndef POLYKERNEL_HPP
#define POLYKERNEL_HPP

#include "FFTTables.hpp"
#include "PolyProc.hpp"

extern "C" void PolyKernel(const APInt32 poly1[PolyProcessor::N],
						   const APTorus32 poly2[PolyProcessor::N],
						   APTorus32 result[PolyProcessor::N],
						   APCplx res[PolyProcessor::N2]);

#endif // POLYKERNEL
