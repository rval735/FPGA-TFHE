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

// M_PI isn't defined with C99 for whatever reason
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

// Private data structure
struct FFTTables
{
	constexpr static uint64_t FFTSize = 2048;
	uint64_t bitReversed[FFTSize];
	double trigTables[(FFTSize - 4) * 2];

	FFTTables(bool isInverse);

	static double accurateSin(uint64_t i, uint64_t n);
	static int32_t floorLog2(uint64_t n);
	static uint64_t reverseBits(uint64_t x, uint32_t n);

	static void fftForward(const FFTTables &tbl, double *real, double *imag);
	static void fftInverse(const FFTTables &tbl, double *real, double *imag);
};

#endif // FFTTABLES_HPP
