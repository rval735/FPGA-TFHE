// Project FPGA-TFHE
// https://github.com/rval735/FPGA-TFHE
//
// Based on code from project
// https://github.com/tfhe/tfhe/
// Copyright 2016 - Nicolas Gama <nicolas.gama@gmail.com> et al.
//
//                    GNU GENERAL PUBLIC LICENSE
//                       Version 3, 29 June 2007
//
// Copyright (C) 2007 Free Software Foundation, Inc. <https://fsf.org/>
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.

#ifndef _DATA_PATH_H_
#define _DATA_PATH_H_

#include <ap_fixed.h>
#include <complex>
#include <vt_fft.hpp>
#include <vt_fft_L2.hpp>

using namespace xf::dsp::fft;
using namespace hls;

// Define number of FFTs, FFT Size, and Super Sample Rate
#define N_FFT 32
#define FFT_LEN 32
#define SSR 16

/// In this example the Vitis HLS ap_fixed type is used to define an 18-bit variable
/// with 6 bits representing the numbers above the decimal point and 12-bits representing
/// the value below the decimal point. The variable is specified as signed, the
/// quantization mode is set to round to plus infinity and the default wrap-around mode
/// is used for overflow.
/// ap_fixed<18,6,AP_RND > my_type;
#define IN_WL 16
#define IN_IL 2
#define TW_WL 16
#define TW_IL 2
#define IID 0

// Define parameter structure for FFT
struct FFTForward : ssr_fft_default_params {
    static const int NUM_FFT_MAX = N_FFT;
    static const int N = FFT_LEN;
    static const int R = SSR;
    static const scaling_mode_enum scaling_mode = SSR_FFT_NO_SCALING;
    static const fft_output_order_enum output_data_order = SSR_FFT_NATURAL;
    static const int twiddle_table_word_length = TW_WL;
    static const int twiddle_table_intger_part_length = TW_IL;
    static const transform_direction_enum transform_direction = FORWARD_TRANSFORM;
};

struct FFTInverse : ssr_fft_default_params {
    static const int NUM_FFT_MAX = N_FFT;
    static const int N = FFT_LEN;
    static const int R = SSR;
    static const scaling_mode_enum scaling_mode = SSR_FFT_NO_SCALING;
    static const fft_output_order_enum output_data_order = SSR_FFT_NATURAL;
    static const int twiddle_table_word_length = TW_WL;
    static const int twiddle_table_intger_part_length = TW_IL;
    static const transform_direction_enum transform_direction = REVERSE_TRANSFORM;
};

typedef std::complex<ap_fixed<IN_WL, IN_IL>> FFTType;

#endif // _DATA_PATH_H_
