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

#ifndef TORUSOPS_HPP
#define TORUSOPS_HPP

#include "FFTTables.hpp"
#include "FFTProc.hpp"

extern "C" void FFTL2Kernel(const int32_t poly1[FFTTables::FFTSize],
							const int32_t poly2[FFTTables::FFTSize],
							FFTProcessor *processor,
							cplx lagrange1[FFTTables::FFTSize],
							cplx lagrange2[FFTTables::FFTSize]);

#endif // TORUSOPS
