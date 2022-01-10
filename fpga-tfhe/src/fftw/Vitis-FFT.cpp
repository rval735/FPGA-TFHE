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

void fft_top(hls::stream<T_in> p_inData[SSR], hls::stream<T_out> p_outData[SSR])
{
    xf::dsp::fft::fft<fftParams, IID>(p_inData, p_outData);
}
