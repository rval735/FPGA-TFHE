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

#include <hls_math.h>
#include "FFTTables.hpp"

using namespace std;


// Returns a pointer to an opaque structure of FFT tables. n must be a power of 2 and n >= 4.
FFTTables::FFTTables(bool isInverse)
{
	const size_t n = FFTSize;
	// Precompute bit reversal table
	APInt32 levels = floorLog2(n);
	APUInt64 i;

	for (i = 0; i < n; i++)
	{
		bitReversed[i] = reverseBits(i, levels);
	}

	// Precompute the packed trigonometric table for each FFT internal level
	APUInt64 size;
	APUInt64 k = 0;
	APDouble sign = isInverse ? -1 : 1;

	for (size = 8; size <= n; size *= 2)
	{
		for (i = 0; i < size / 2; i += 4)
		{
			APUInt64 j;
			for (j = 0; j < 4; j++, k++)
			{
				trigTables[k] = accurateSin(i + j + size / 4, size);  // Cosine
			}
			for (j = 0; j < 4; j++, k++)
			{
				trigTables[k] = sign * accurateSin(i + j, size);  // Sine
			}
		}

		if (size == n)
		{
			break;
		}
	}
}

// Returns sin(2 * pi * i / n), for n that is a multiple of 4.
APDouble accurateSin(APUInt64 i, const APUInt64 n)
{
	if (n % 4 != 0)
	{
		return NAN;
	}
	else
	{
		APInt32 neg = 0;  // Boolean
		// Reduce to full cycle
		i %= n;
		// Reduce to half cycle

		if (i >= n / 2)
		{
			neg = 1;
			i -= n / 2;
		}

		// Reduce to quarter cycle
		if (i >= n / 4)
		{
			i = n / 2 - i;
		}

		APDouble val = 0;
		APDouble iD = (APDouble)i;
		APDouble nD = (APDouble)n;
		// Reduce to eighth cycle
		if (i * 8 < n)
		{
			APDouble pre = iD / nD;
			val = sin(2.0 * M_PI * pre);
		}
		else
		{

			APDouble pre = (nD / 4.0 - iD) / nD;
			val = cos(2.0 * M_PI * pre);
		}
		// Apply sign
		if (neg)
		{
			return -1.0 * val;
		}
		else
		{
			return val;
		}
	}
}

// Returns the largest i such that 2^i <= n.
APInt32 floorLog2(const APUInt64 n)
{
	APInt32 result = 0;

	for (APUInt64 i = n; i > 1; i /= 2)
	{
		result++;
	}

	return result;
}


// Returns the bit reversal of the n-bit unsigned integer x.
APUInt64 reverseBits(const APUInt64 x, const APUInt32 n)
{
	APUInt64 result = 0;
	APUInt64 xD = x;

	for (APUInt32 i = 0; i < n; i++, xD >>= 1)
	{
		result = (result << 1) | (xD & 1);
	}

	return result;
}

// Real and Imag sizes are expected to be FFTSize
void fftForward(const FFTTables tablesForward, APDouble real[FFTTables::FFTSize], APDouble imag[FFTTables::FFTSize])
{
	APUInt64 n = FFTTables::FFTSize;

	// Bit-reversed addressing permutation
	APUInt64 i;

	for (i = 0; i < n; i++)
	{
		APUInt64 j = tablesForward.bitReversed[i];

		if (i < j)
		{
			APDouble tp0re = real[i];
			APDouble tp0im = imag[i];
			APDouble tp1re = real[j];
			APDouble tp1im = imag[j];
			real[i] = tp1re;
			imag[i] = tp1im;
			real[j] = tp0re;
			imag[j] = tp0im;
		}

//		cout << "ElemsU: " << real[i] << ", " << imag[i]<< endl;
	}

	// Size 2 merge (special)
	if (n >= 2)
	{
		for (i = 0; i < n; i += 2)
		{
			APDouble tpre = real[i];
			APDouble tpim = imag[i];
			real[i] += real[i + 1];
			imag[i] += imag[i + 1];
			real[i + 1] = tpre - real[i + 1];
			imag[i + 1] = tpim - imag[i + 1];
		}
	}

	// Size 4 merge (special)
	if (n >= 4)
	{
		for (i = 0; i < n; i += 4)
		{
			// Even indices
			APDouble tpre, tpim;
			tpre = real[i];
			tpim = imag[i];
			real[i] += real[i + 2];
			imag[i] += imag[i + 2];
			real[i + 2] = tpre - real[i + 2];
			imag[i + 2] = tpim - imag[i + 2];
			// Odd indices
			tpre = real[i + 1];
			tpim = imag[i + 1];
			real[i + 1] += imag[i + 3];
			imag[i + 1] -= real[i + 3];
			tpre -= imag[i + 3];
			tpim += real[i + 3];
			real[i + 3] = tpre;
			imag[i + 3] = tpim;
		}
	}

	// Size 8 and larger merges (general)
	APUInt64 size;
	APUInt64 tblOffset = 0;

	for (size = 8; size <= n; size <<= 1)
	{
		APUInt64 halfsize = size >> 1;
		APUInt64 i;

		for (i = 0; i < n; i += size)
		{
			APUInt64 j, off;
			for (j = 0, off = 0; j < halfsize; j += 4, off += 8)
			{
				APUInt64 k;
				for (k = 0; k < 4; k++)
				{  // To simulate x86 AVX 4-vectors
					APUInt64 vi = i + j + k;  // Vector index
					APUInt64 ti = off + k;    // Table index
					APDouble re = real[vi + halfsize];
					APDouble im = imag[vi + halfsize];
					APDouble tpre = re * tablesForward.trigTables[tblOffset + ti] + im * tablesForward.trigTables[tblOffset + ti + 4];
					APDouble tpim = im * tablesForward.trigTables[tblOffset + ti] - re * tablesForward.trigTables[tblOffset + ti + 4];
					real[vi + halfsize] = real[vi] - tpre;
					imag[vi + halfsize] = imag[vi] - tpim;
					real[vi] += tpre;
					imag[vi] += tpim;
				}
			}
		}

		if (size == n)
		{
			break;
		}

		tblOffset += size;
	}
}

// Real and Imag sizes are expected to be FFTSize
void fftInverse(const FFTTables tablesInverse, APDouble real[FFTTables::FFTSize], APDouble imag[FFTTables::FFTSize])
{
	APUInt64 n = FFTTables::FFTSize;

	// Bit-reversed addressing permutation
	int i;

	for (i = 0; i < n; i++)
	{
		APUInt64 j = tablesInverse.bitReversed[i];
		if (i < j)
		{
			APDouble tp0re = real[i];
			APDouble tp0im = imag[i];
			APDouble tp1re = real[j];
			APDouble tp1im = imag[j];
			real[i] = tp1re;
			imag[i] = tp1im;
			real[j] = tp0re;
			imag[j] = tp0im;
		}

//		cout << "ElemsU: " << real[1] << ", " << imag[1]<< endl;
	}

	// Size 2 merge (special)
	if (n >= 2)
	{
		for (i = 0; i < n; i += 2)
		{
			APDouble tpre = real[i];
			APDouble tpim = imag[i];
			real[i] += real[i + 1];
			imag[i] += imag[i + 1];
			real[i + 1] = tpre - real[i + 1];
			imag[i + 1] = tpim - imag[i + 1];
		}
	}

	// Size 4 merge (special)
	if (n >= 4)
	{
		for (i = 0; i < n; i += 4)
		{
			// Even indices
			APDouble tpre, tpim;
			tpre = real[i];
			tpim = imag[i];
			real[i] += real[i + 2];
			imag[i] += imag[i + 2];
			real[i + 2] = tpre - real[i + 2];
			imag[i + 2] = tpim - imag[i + 2];
			// Odd indices
			tpre = real[i + 1];
			tpim = imag[i + 1];
			real[i + 1] -= imag[i + 3];
			imag[i + 1] += real[i + 3];
			tpre += imag[i + 3];
			tpim -= real[i + 3];
			real[i + 3] = tpre;
			imag[i + 3] = tpim;
		}
	}

	// Size 8 and larger merges (general)
	APUInt64 size;
	APUInt64 tblOffset = 0;

	for (size = 8; size <= n; size <<= 1)
	{
		APUInt64 halfsize = size >> 1;
		APUInt64 i;

		for (i = 0; i < n; i += size)
		{
			APUInt64 j, off;

			for (j = 0, off = 0; j < halfsize; j += 4, off += 8)
			{
				APUInt64 k;
				for (k = 0; k < 4; k++)
				{  // To simulate x86 AVX 4-vectors
					APUInt64 vi = i + j + k;  // Vector index
					APUInt64 ti = off + k;    // Table index
					APDouble re = real[vi + halfsize];
					APDouble im = imag[vi + halfsize];
					APDouble tpre = re * tablesInverse.trigTables[tblOffset + ti] + im * tablesInverse.trigTables[tblOffset + ti + 4];
					APDouble tpim = im * tablesInverse.trigTables[tblOffset + ti] - re * tablesInverse.trigTables[tblOffset + ti + 4];
					real[vi + halfsize] = real[vi] - tpre;
					imag[vi + halfsize] = imag[vi] - tpim;
					real[vi] += tpre;
					imag[vi] += tpim;
				}
			}
		}

		if (size == n)
		{
			break;
		}

		tblOffset += size;
	}
}
