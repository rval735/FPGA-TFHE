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

#ifndef VITISFFT
#define VITISFFT

#include <xcl/xcl2.hpp>
#include <xf_utils_sw/logger.hpp>
#include <utility> // std::pair
#include "FFTProc.hpp"

#define OCLFLAGS	CL_QUEUE_PROFILING_ENABLE | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE
//#define OCLFLAGS	CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE

// echo 2 | sudo tee /sys/module/hid_apple/parameters/fnmode
struct OCLFFT
{
	OCLFFT(std::string xclbinPath);
	void executeFFT();
	void executeFFTAlt();

	static void ocl_check(const cl_int &err);

	cl::Device device;
	cl::Context context;
	cl::CommandQueue cmdQ;
	cl::Program program;
	cl::Kernel kernel;
	xf::common::utils_sw::Logger logger = xf::common::utils_sw::Logger(std::cout, std::cerr);
};

std::pair<cplx *, int32_t*> cpuFFT(const int &n);
std::pair<cplx *, int32_t*> fpgaFFT(OCLFFT *oclFFT, const int &n);


/**
 * FFT functions
// */
//EXPORT void IntPolynomial_ifft(LagrangeHalfCPolynomial* result, const IntPolynomial* p) {
//    LagrangeHalfCPolynomial_IMPL* r = (LagrangeHalfCPolynomial_IMPL*) result;
//    fp1024_nayuki.execute_reverse_int(r->coefsC, p->coefs);
//}
//EXPORT void TorusPolynomial_ifft(LagrangeHalfCPolynomial* result, const TorusPolynomial* p) {
//    LagrangeHalfCPolynomial_IMPL* r = (LagrangeHalfCPolynomial_IMPL*) result;
//    fp1024_nayuki.execute_reverse_torus32(r->coefsC, p->coefsT);
//}
//EXPORT void TorusPolynomial_fft(TorusPolynomial* result, const LagrangeHalfCPolynomial* p) {
//    LagrangeHalfCPolynomial_IMPL* r = (LagrangeHalfCPolynomial_IMPL*) p;
//    fp1024_nayuki.execute_direct_torus32(result->coefsT, r->coefsC);
//}

#endif // VITISFFT
