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
#include <PolyProc.hpp>
#include "fftw/lagrangehalfc_impl.h"

#define OCLFLAGS	CL_QUEUE_PROFILING_ENABLE | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE
//#define OCLFLAGS	CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE
//#define OCLFLAGS	CL_QUEUE_PROFILING_ENABLE
//#define OCLFLAGS 0


struct OCLPoly
{
	OCLPoly(std::string xclbinPath);
	~OCLPoly();
	void polyKernel(TorusPolynomial *result, const IntPolynomial *poly1, const TorusPolynomial *poly2);
	void testOp();

	static void ocl_check(const cl_int &err);

	cl::Device device;
	cl::Context context;
	cl::CommandQueue cmdQ;
	cl::Program program;
	cl::Kernel kernel;
	xf::common::utils_sw::Logger logger = xf::common::utils_sw::Logger(std::cout, std::cerr);

	std::vector<APInt32, aligned_allocator<APInt32>> poly1T;
	std::vector<APTorus32, aligned_allocator<APTorus32>> poly2T;
	std::vector<APTorus32, aligned_allocator<APTorus32>> resultT;

	cl::Buffer poly1TBuff;
	cl::Buffer poly2TBuff;
	cl::Buffer resultBuff;

	cl_mem_ext_ptr_t inBufExt1, inBufExt2, outBufExt;
};

int tvdiff(struct timeval* tv0, struct timeval* tv1);

#endif // VITISPOLY
