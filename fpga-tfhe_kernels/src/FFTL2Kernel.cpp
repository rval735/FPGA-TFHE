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

static FFTProcessor processor;

extern "C" void FFTL2Kernel(const int32_t inData[FFT_LEN * N_FFT / SSR],
							cplx outData[FFT_LEN * N_FFT / SSR],
							int nFrames,
							bool isForward)
{
	processor.executeReverseInt(outData, inData);
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
