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


#include <complex>

#include "FFTProc.hpp"
//#include "tfhe/polynomials.h"
//#include "lagrangehalfc_impl.h"
//#include "fft.h"

void executeReverseInt(FFTProcessor *proc, cplx res[FFTProcessor::N], const int32_t a[FFTProcessor::N])
{
	int n = FFTProcessor::N;
	int n2 = FFTProcessor::N2;
	double *res_dbl=(double *)res;

    for (int32_t i = 0; i < n; i++)
    {
    	proc->realInOut[i] = a[i]/2.;
    }

    for (int32_t i = 0; i < n; i++)
    {
    	proc->realInOut[n + i] =-proc->realInOut[i];
    }

    for (int32_t i = 0; i < n2; i++)
    {
    	proc->imagInOut[i] = 0;
    }

    fftInverse(&proc->tablesInverse, proc->realInOut, proc->imagInOut);

    for (int32_t i = 0; i < n; i += 2)
    {
    	res_dbl[i] = proc->realInOut[i + 1];
    	res_dbl[i + 1] = proc->imagInOut[i + 1];
    }
}

void executeReverseTorus32(FFTProcessor *proc, cplx res[FFTProcessor::N], const Torus32 a[FFTProcessor::N])
{
    static const double _2pm33 = 1./double(INT64_C(1)<<33);
    int n = FFTProcessor::N;
    int n2 = FFTProcessor::N2;
    int32_t *aa = (int32_t *)a;

    for (int32_t i = 0; i < n; i++)
    {
    	proc->realInOut[i] = aa[i] * _2pm33;
    }

    for (int32_t i = 0; i < n; i++)
    {
    	proc->realInOut[n + i] = -proc->realInOut[i];
    }

    for (int32_t i = 0; i < n2; i++)
    {
    	proc->imagInOut[i] = 0;
    }

    fftInverse(&proc->tablesInverse, proc->realInOut, proc->imagInOut);

    for (int32_t i = 0; i < FFTProcessor::Ns2; i++)
    {
    	res[i] = cplx(proc->realInOut[2 * i + 1], proc->imagInOut[2 * i + 1]);
    }
}

void executeDirectTorus32(FFTProcessor *proc, Torus32 res[FFTProcessor::N], const cplx a[FFTProcessor::N])
{
    static const double _2p32 = double(INT64_C(1)<<32);
    int n = FFTProcessor::N;
    int n2 = FFTProcessor::N2;
    int ns2 = FFTProcessor::Ns2;
    static const double _1sN = double(1)/double(n);

    for (int32_t i = 0; i < n; i++)
    {
    	proc->realInOut[2 * i] = 0;
    }

    for (int32_t i = 0; i < n; i++)
    {
    	proc->imagInOut[2 * i] = 0;
    }

    for (int32_t i = 0; i < ns2; i++)
    {
    	proc->realInOut[2 * i + 1] = real(a[i]);
    }

    for (int32_t i = 0; i < ns2; i++)
    {
    	proc->imagInOut[2 * i + 1] = imag(a[i]);
    }

    for (int32_t i = 0; i < ns2; i++)
    {
    	proc->realInOut[n2 - 1 - 2 * i] = real(a[i]);
    }

    for (int32_t i = 0; i < ns2; i++)
    {
    	proc->imagInOut[n2 - 1 - 2 * i] = -imag(a[i]);
    }

    fftForward(&proc->tablesForward, proc->realInOut, proc->imagInOut);

    for (int32_t i = 0; i < n; i++)
    {
    	res[i] = Torus32(int64_t(proc->realInOut[i] * _1sN * _2p32));
    }
}


//thread_local FFT_Processor_nayuki fp1024_nayuki(1024);

///**
// * FFT functions
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
