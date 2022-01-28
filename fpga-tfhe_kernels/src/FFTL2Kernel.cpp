/*
 * Copyright 2019 Xilinx, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.

*/
//================================== End Lic =================================================
#include "FFTL2Kernel.hpp"

extern "C" void FFTL2Kernel(ap_uint<512> inData[FFT_LEN * N_FFT / SSR],
							ap_uint<512> outData[FFT_LEN * N_FFT / SSR],
							int nFrames,
							bool isForward)
{
#pragma HLS interface m_axi port inData = gmem0 offset = slave
#pragma HLS interface s_axilite port = inData bundle = control

#pragma HLS interface m_axi port outData = gmem0 offset = slave
#pragma HLS interface s_axilite port = outData bundle = control

#pragma HLS interface s_axilite port = nFrames bundle = control
#pragma HLS interface s_axilite port = return bundle = control

#pragma HLS interface s_axilite port = isForward bundle = control
#pragma HLS interface s_axilite port = return bundle = control

	if (isForward == true)
	{
		xf::dsp::fft::fftKernel<FFTForward, IID, FFTType>(inData, outData, nFrames);
	}
	else
	{
		xf::dsp::fft::fftKernel<FFTInverse, IID, FFTType>(inData, outData, nFrames);
	}
}
