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


#include "FFTTables.hpp"

#include <math.h>

// Returns a pointer to an opaque structure of FFT tables. n must be a power of 2 and n >= 4.
void FFTTables::FFTTables(bool isInverse)
{
	size_t n = FFTSize;
	// Precompute bit reversal table
	int32_t levels = FFTTables::floorLog2(n);
	uint64_t i;

	for (i = 0; i < n; i++)
	{
		tables.bit_reversed[i] = reverse_bits(i, levels);
	}

	// Precompute the packed trigonometric table for each FFT internal level
	uint64_t size;
	uint64_t k = 0;
	double sign = isInverse ? -1 : 1;
	for (size = 8; size <= n; size *= 2)
	{
		for (i = 0; i < size / 2; i += 4)
		{
			uint64_t j;
			for (j = 0; j < 4; j++, k++)
			{
				tables.trig_tables[k] = accurate_sine(i + j + size / 4, size);  // Cosine
			}
			for (j = 0; j < 4; j++, k++)
			{
				tables.trig_tables[k] = sign * accurate_sine(i + j, size);  // Sine
			}
		}

		if (size == n)
		{
			break;
		}
	}
}

// Returns sin(2 * pi * i / n), for n that is a multiple of 4.
static double FFTTables::accurateSin(uint64_t i, uint64_t n)
{
	if (n % 4 != 0)
	{
		return NAN;
	}
	else
	{
		int32_t neg = 0;  // Boolean
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
		double val;
		// Reduce to eighth cycle
		if (i * 8 < n)
		{
			val = sin(2 * M_PI * i / n);
		}
		else
		{
			val = cos(2 * M_PI * (n / 4 - i) / n);
		}
		// Apply sign
		return neg ? -val : val;
	}
}

// Returns the largest i such that 2^i <= n.
static int32_t FFTTables::floorLog2(uint64_t n)
{
	int32_t result = 0;
	for (; n > 1; n /= 2)
	{
		result++;
	}

	return result;
}


// Returns the bit reversal of the n-bit unsigned integer x.
static uint64_t FFTTables::reverseBits(uint64_t x, uint32_t n)
{
	uint64_t result = 0;
	uint32_t i;
	for (i = 0; i < n; i++, x >>= 1)
	{
		result = (result << 1) | (x & 1);
	}

	return result;
}

// Real and Imag sizes are expected to be FFTSize
void FFTTables::fftForward(const FFTTables &tbl, double *real, double *imag)
{
	uint64_t n = tbl.FFTSize;

	// Bit-reversed addressing permutation
	uint64_t i;
	uint64_t *bitreversed = tbl.bit_reversed;

	for (i = 0; i < n; i++)
	{
		uint64_t j = bitreversed[i];

		if (i < j)
		{
			double tp0re = real[i];
			double tp0im = imag[i];
			double tp1re = real[j];
			double tp1im = imag[j];
			real[i] = tp1re;
			imag[i] = tp1im;
			real[j] = tp0re;
			imag[j] = tp0im;
		}
	}

	// Size 2 merge (special)
	if (n >= 2)
	{
		for (i = 0; i < n; i += 2)
		{
			double tpre = real[i];
			double tpim = imag[i];
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
			double tpre, tpim;
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
	double *trigtables = tbl.trig_tables;
	uint64_t size;

	for (size = 8; size <= n; size <<= 1)
	{
		uint64_t halfsize = size >> 1;
		uint64_t i;

		for (i = 0; i < n; i += size)
		{
			uint64_t j, off;
			for (j = 0, off = 0; j < halfsize; j += 4, off += 8)
			{
				uint64_t k;
				for (k = 0; k < 4; k++)
				{  // To simulate x86 AVX 4-vectors
					uint64_t vi = i + j + k;  // Vector index
					uint64_t ti = off + k;    // Table index
					double re = real[vi + halfsize];
					double im = imag[vi + halfsize];
					double tpre = re * trigtables[ti] + im * trigtables[ti + 4];
					double tpim = im * trigtables[ti] - re * trigtables[ti + 4];
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

		trigtables += size;
	}
}

// Real and Imag sizes are expected to be FFTSize
void FFTTables::fftInverse(const FFTTables &tbl, double *real, double *imag)
{
	uint64_t n = tbl->n;

	// Bit-reversed addressing permutation
	uint64_t i;
	uint64_t *bitreversed = tbl->bit_reversed;

	for (i = 0; i < n; i++)
	{
		uint64_t j = bitreversed[i];
		if (i < j)
		{
			double tp0re = real[i];
			double tp0im = imag[i];
			double tp1re = real[j];
			double tp1im = imag[j];
			real[i] = tp1re;
			imag[i] = tp1im;
			real[j] = tp0re;
			imag[j] = tp0im;
		}
	}

	// Size 2 merge (special)
	if (n >= 2)
	{
		for (i = 0; i < n; i += 2)
		{
			double tpre = real[i];
			double tpim = imag[i];
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
			double tpre, tpim;
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
	double *trigtables = tbl->trig_tables;
	uint64_t size;

	for (size = 8; size <= n; size <<= 1)
	{
		uint64_t halfsize = size >> 1;
		uint64_t i;

		for (i = 0; i < n; i += size)
		{
			uint64_t j, off;

			for (j = 0, off = 0; j < halfsize; j += 4, off += 8)
			{
				uint64_t k;
				for (k = 0; k < 4; k++)
				{  // To simulate x86 AVX 4-vectors
					uint64_t vi = i + j + k;  // Vector index
					uint64_t ti = off + k;    // Table index
					double re = real[vi + halfsize];
					double im = imag[vi + halfsize];
					double tpre = re * trigtables[ti] + im * trigtables[ti + 4];
					double tpim = im * trigtables[ti] - re * trigtables[ti + 4];
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

		trigtables += size;
	}
}
