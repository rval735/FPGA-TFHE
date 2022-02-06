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

#ifndef FFTTABLES_HPP
#define FFTTABLES_HPP

#include <stdint.h>

// M_PI isn't defined with C99 for whatever reason
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

/// In this example the Vitis HLS ap_fixed type is used to define an 18-bit variable
/// with 6 bits representing the numbers above the decimal point and 12-bits representing
/// the value below the decimal point. The variable is specified as signed, the
/// quantization mode is set to round to plus infinity and the default wrap-around mode
/// is used for overflow.
/// ap_fixed<18,6,AP_RND > my_type;
#define IN_WL 16
#define IN_IL 2

typedef std::complex<ap_fixed<IN_WL, IN_IL>> FFTType;

// Private data structure
struct FFTTables
{
	constexpr static uint64_t FFTSize = 2048;
	uint64_t bitReversed[FFTSize];
	double trigTables[(FFTSize - 4) * 2];

	FFTTables(bool isInverse);
};

double accurateSin(uint64_t i, uint64_t n);
int32_t floorLog2(uint64_t n);
uint64_t reverseBits(uint64_t x, uint32_t n);

void fftForward(const FFTTables *tbl, double *real, double *imag);
void fftInverse(const FFTTables *tbl, double *real, double *imag);

#endif // FFTTABLES_HPP
