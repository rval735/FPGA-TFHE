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
#include <complex>

#include "FFTTables.hpp"
#include "FFTProc.hpp"
#include "FFTL2Kernel.hpp"
#include "fftw/lagrangehalfc_impl.h"

// typedef double _Complex cplx;
typedef std::complex<double> cplx; // https://stackoverflow.com/a/31800404

//#define OCLFLAGS	CL_QUEUE_PROFILING_ENABLE | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE
//#define OCLFLAGS	CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE
#define OCLFLAGS	CL_QUEUE_PROFILING_ENABLE

using namespace std;

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

tuple<vector<cplx>, vector<int32_t>, FFT_Processor_nayuki *> cpuFFT(const int &n);
tuple<vector<APCplx>, vector<APInt32>, FFTProcessor *> kernelFFT(const int &n);
pair<APCplx *, APInt32 *> fpgaFFT(OCLFFT *oclFFT, const int &n);


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



//EXPORT void IntPolynomial_ifft(LagrangeHalfCPolynomial* result, const IntPolynomial* p) {
//    LagrangeHalfCPolynomial_IMPL* r = (LagrangeHalfCPolynomial_IMPL*) result;
//    APCplx coefsC[FFTProcessor::N];
//    APInt32 coefsP[FFTProcessor::N];
//
//    for (int i = 0; i < FFTProcessor::N; i++)
//    {
////    	coefsC[i] = r->coefsC[i];
////    	coefsP[i] = p->coefs[i];
////    	double coefCR = r->coefsC[i].real();
////		double coefCI = r->coefsC[i].imag();
//		coefsP[i] = p->coefs[i];
//
////		if (isnan(coefCR) == false)
////		{
////			coefsC[i].real(coefCR);
////		}
////
////		if (isnan(coefCI) == false)
////		{
////			coefsC[i].imag(coefCI);
////		}
//    }
//
////    fp1024_nayuki.execute_reverse_int(r->coefsC, p->coefs);
//    executeReverseInt(r->proc, coefsC, coefsP);
//
//    for (int i = 0; i < FFTProcessor::N; i++)
//	{
//    	double real = coefsC[i].real().to_double();
//    	double imag = coefsC[i].imag().to_double();
//    	r->coefsC[i].real(real);
//    	r->coefsC[i].imag(imag);
////    	p->coefs[i] = coefsP[i];
//	}
//}
//
//EXPORT void TorusPolynomial_ifft(LagrangeHalfCPolynomial* result, const TorusPolynomial* p) {
//    LagrangeHalfCPolynomial_IMPL* r = (LagrangeHalfCPolynomial_IMPL*) result;
//    APCplx coefsC[FFTProcessor::N];
//    APInt32 coefsP[FFTProcessor::N];
//
//    for (int i = 0; i < FFTProcessor::N; i++)
//    {
////    	double coefCR = r->coefsC[i].real();
////    	double coefCI = r->coefsC[i].imag();
//    	coefsP[i] = p->coefsT[i];
//
////    	if (isnan(coefCR) == false)
////    	{
////    		coefsC[i].real(coefCR);
////    	}
////
////    	if (isnan(coefCI) == false)
////    	{
////    		coefsC[i].imag(coefCI);
////    	}
//    }
//
////    fp1024_nayuki.execute_reverse_torus32(r->coefsC, p->coefsT);
//    executeReverseTorus32(r->proc, coefsC, coefsP);
//
//    for (int i = 0; i < FFTProcessor::N; i++)
//	{
//    	double real = coefsC[i].real().to_double();
//		double imag = coefsC[i].imag().to_double();
//    	r->coefsC[i].real(real);
//    	r->coefsC[i].imag(imag);
////		p->coefsT[i] = coefsP[i];
//	}
//}
//
//EXPORT void TorusPolynomial_fft(TorusPolynomial* result, const LagrangeHalfCPolynomial* p) {
//    LagrangeHalfCPolynomial_IMPL* r = (LagrangeHalfCPolynomial_IMPL*) p;
//    APCplx coefsC[FFTProcessor::N];
//    APInt32 coefsP[FFTProcessor::N];
//
//    for (int i = 0; i < FFTProcessor::N; i++)
//    {
////    	coefsC[i] = r->coefsC[i];
////    	coefsP[i] = result->coefsT[i];
//    	double coefCR = r->coefsC[i].real();
//		double coefCI = r->coefsC[i].imag();
////		coefsP[i] = result->coefsT[i];
//
//		if (isnan(coefCR) == false)
//		{
//			coefsC[i].real(coefCR);
//		}
//
//		if (isnan(coefCI) == false)
//		{
//			coefsC[i].imag(coefCI);
//		}
//    }
//
////    fp1024_nayuki.execute_direct_torus32(result->coefsT, r->coefsC);
////    executeDirectTorus32(r->proc, coefsP, coefsC);
//
//    for (int i = 0; i < FFTProcessor::N; i++)
//	{
//		result->coefsT[i] = coefsP[i];
////		double real = coefsC[i].real().to_double();
////		double imag = coefsC[i].imag().to_double();
////		r->coefsC[i] = std::complex<double>(real, imag);
//	}
//}

#endif // VITISFFT
