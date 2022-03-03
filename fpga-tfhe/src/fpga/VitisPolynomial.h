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

#ifndef VITISPOLY
#define VITISPOLY

#include <xcl/xcl2.hpp>
#include <xf_utils_sw/logger.hpp>
#include <utility> // std::pair
#include <complex>

#include <PolyKernel.hpp>
#include "fftw/lagrangehalfc_impl.h"

using namespace std;

// typedef double _Complex cplx;
typedef std::complex<double> cplx; // https://stackoverflow.com/a/31800404

//#define OCLFLAGS	CL_QUEUE_PROFILING_ENABLE | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE
//#define OCLFLAGS	CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE
#define OCLFLAGS	CL_QUEUE_PROFILING_ENABLE


struct OCLPoly
{
	OCLPoly(std::string xclbinPath);
	void torusPolynomialAddMulRFFT(TorusPolynomial *result, const IntPolynomial *poly1, const TorusPolynomial *poly2);

	static void ocl_check(const cl_int &err);

	cl::Device device;
	cl::Context context;
	cl::CommandQueue cmdQ;
	cl::Program program;
	cl::Kernel kernel;
	xf::common::utils_sw::Logger logger = xf::common::utils_sw::Logger(std::cout, std::cerr);

	APInt32 *poly1T;
	APTorus32 *poly2T;
	APTorus32 *resultT;

	cl::Buffer poly1TBuff;
	cl::Buffer poly2TBuff;
	cl::Buffer resultBuff;
};

int tvdiff(struct timeval* tv0, struct timeval* tv1);

#endif // VITISPOLY
