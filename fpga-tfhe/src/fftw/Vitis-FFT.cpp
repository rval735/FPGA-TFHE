// Project FPGA-TFHE
// https://github.com/rval735/FPGA-TFHE
//
//                    GNU GENERAL PUBLIC LICENSE
//                       Version 3, 29 June 2007
//
// Copyright (C) 2007 Free Software Foundation, Inc. <https://fsf.org/>
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.

#include "Vitis-FFT.h"

void fft_top(hls::stream<T_in> p_inData[SSR], hls::stream<T_out_F> p_outData[SSR])
{
    xf::dsp::fft::fft<fftForward, IID>(p_inData, p_outData);
}

void fft_top(hls::stream<T_out_F> p_inData[SSR], hls::stream<T_out_I> p_outData[SSR])
{
    xf::dsp::fft::fft<fftInverse, IID>(p_inData, p_outData);
}
