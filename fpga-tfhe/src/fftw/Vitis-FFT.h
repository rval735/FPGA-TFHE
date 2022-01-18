// Project FPGA-TFHE
// https://github.com/rval735/FPGA-TFHE
//
//                    GNU GENERAL PUBLIC LICENSE
//                       Version 3, 29 June 2007
//
// Copyright (C) 2007 Free Software Foundation, Inc. <https://fsf.org/>
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.

///Vitis_Libraries/dsp/L1/include/hw/vitis_fft/fixed

#ifndef _VITIS-FFT_H_
#define _VITIS-FFT_H_

#include <ap_fixed.h>
#include <complex>
#include <hls_stream.h>
#include <vt_fft.hpp>

using namespace xf::dsp::fft;

// Define FFT Size and Super Sample Rate
#define FFT_LEN 16
#define SSR 4

#define IN_WL 16
#define IN_IL 2
#define TW_WL 16
#define TW_IL 2
#define IID 0

typedef std::complex<ap_fixed<IN_WL, IN_IL> > T_in;

// Define parameter structure for FFT
struct fftParams : ssr_fft_default_params {
    static const int N = FFT_LEN;
    static const int R = SSR;
    static const scaling_mode_enum scaling_mode = SSR_FFT_NO_SCALING;
    static const fft_output_order_enum output_data_order = SSR_FFT_NATURAL;
    static const int twiddle_table_word_length = TW_WL;
    static const int twiddle_table_intger_part_length = TW_IL;
};

typedef ssr_fft_output_type<fftParams, T_in>::t_ssr_fft_out T_out;

void fft_top(hls::stream<T_in> p_inData[SSR], hls::stream<T_out> p_outData[SSR]);

#endif // _VITIS-FFT_H_
