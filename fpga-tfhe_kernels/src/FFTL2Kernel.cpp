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
#include <iostream>

static FFTProcessor proc[1];

extern "C" void FFTL2Kernel(const APInt32 poly1[FFTProcessor::N],
							const APTorus32 poly2[FFTProcessor::N],
							APTorus32 result[FFTProcessor::N]
							//APCplx res[FFTProcessor::N]
							)
{


	for (int i = 0; i < FFTProcessor::N; i++)
	{
		std::cout << i << "\t\t" << poly1[i] << "\t\t" << poly2[i] << std::endl;
	}

	std::cout << "/////////////////////////" << std::endl;

	APCplx tmp0[FFTProcessor::N];
	APCplx tmp1[FFTProcessor::N];
	APCplx tmp2[FFTProcessor::N];
	APTorus32 tmpT[FFTProcessor::N];

	executeReverseInt(proc, tmp0, poly1);
//	executeReverseInt(proc, res, poly1);
	executeReverseTorus32(proc, tmp1, poly2);
	lagrangeHalfCPolynomialMul(tmp2, tmp0, tmp1);
	executeDirectTorus32(proc, tmpT, tmp2);
	torusPolynomialAddTo(result, tmpT);

	for (int i = 0; i < FFTProcessor::N; i++)
	{
		std::cout << i << "\t\t" << result[i] << std::endl;
	}

//	EXPORT void torusPolynomialAddMulRFFT(TorusPolynomial* result, const IntPolynomial* poly1, const TorusPolynomial* poly2)
//	    const int32_t N = poly1->N;
//	    LagrangeHalfCPolynomial* tmp = new_LagrangeHalfCPolynomial_array(3,N);
//	    TorusPolynomial* tmpr = new_TorusPolynomial(N);
//	    IntPolynomial_ifft(tmp+0,poly1);
//	    TorusPolynomial_ifft(tmp+1,poly2);
//	    LagrangeHalfCPolynomialMul(tmp+2,tmp+0,tmp+1);
//	    TorusPolynomial_fft(tmpr, tmp+2);
//	    torusPolynomialAddTo(result, tmpr);
//	    delete_TorusPolynomial(tmpr);
//	    delete_LagrangeHalfCPolynomial_array(3,tmp);


//#pragma HLS interface m_axi port inData = gmem0 offset = slave
//#pragma HLS interface s_axilite port = inData bundle = control
//
//#pragma HLS interface m_axi port outData = gmem0 offset = slave
//#pragma HLS interface s_axilite port = outData bundle = control
//
//#pragma HLS interface s_axilite port = nFrames bundle = control
//#pragma HLS interface s_axilite port = return bundle = control
//
//#pragma HLS interface s_axilite port = isForward bundle = control
//#pragma HLS interface s_axilite port = return bundle = control
//
//	if (isForward == true)
//	{
//		xf::dsp::fft::fftKernel<FFTForward, IID, FFTType>(inData, outData, nFrames);
////		for(int i = 0; i < FFT_LEN * N_FFT / SSR; i++)
////		{
////			std::cout << "f(" << i << "," << inData[i] << "," << outData[i] << ")\n";
////		}
//
//		for (int n = 0; n < N_FFT; ++n) {
//			for (int t = 0; t < FFT_LEN / SSR; ++t) {
//				for (int r = 0; r < SSR; r++) {
//					if (outData[n * FFT_LEN / SSR + t].range(TW_WL - 1 + TW_WL * 2 * r, TW_WL * 2 * r) != 1 ||
//						outData[n * FFT_LEN / SSR + t].range(TW_WL * 2 - 1 + TW_WL * 2 * r, TW_WL + TW_WL * 2 * r) != 0) {
//						std::cout << "fReal = "
//								  << outData[n * FFT_LEN / SSR + t].range(TW_WL - 1 + TW_WL * 2 * r, TW_WL * 2 * r)
//								  << "    Imag = "
//								  << outData[n * FFT_LEN / SSR + t].range(TW_WL * 2 - 1 + TW_WL * 2 * r,
//																		  TW_WL + TW_WL * 2 * r)
//								  << std::endl;
//					}
//				}
//			}
//		}
//	}
//	else
//	{
//		xf::dsp::fft::fftKernel<FFTInverse, IID, FFTType>(inData, outData, nFrames);
////		for(int i = 0; i < FFT_LEN * N_FFT / SSR; i++)
////		{
////			std::cout << "i(" << i << "," << inData[i] << "," << outData[i] << ")\n";
////		}
//
//		for (int n = 0; n < N_FFT; ++n) {
//			for (int t = 0; t < FFT_LEN / SSR; ++t) {
//				for (int r = 0; r < SSR; r++) {
//					std::cout << "iReal = "
//							  << outData[n * FFT_LEN / SSR + t].range(TW_WL - 1 + TW_WL * 2 * r, TW_WL * 2 * r)
//							  << "    Imag = "
//							  << outData[n * FFT_LEN / SSR + t].range(TW_WL * 2 - 1 + TW_WL * 2 * r,
//																	  TW_WL + TW_WL * 2 * r)
//							  << std::endl;
//				}
//			}
//		}
//	}
}
