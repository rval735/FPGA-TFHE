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


#include "FFTProc.hpp"
//#include "tfhe/polynomials.h"
//#include "lagrangehalfc_impl.h"
//#include "fft.h"

void executeReverseInt(FFTProcessor *proc, APCplx res[FFTProcessor::N], const APInt32 a[FFTProcessor::N])
{
	int n = FFTProcessor::N;
	int n2 = FFTProcessor::N2;
	APDouble *res_dbl=(APDouble *)res;

    for (int i = 0; i < n; i++)
    {
    	proc->realInOut[i] = a[i]/2.;
    }

    for (int i = 0; i < n; i++)
    {
    	proc->realInOut[n + i] =-proc->realInOut[i];
    }

    for (int i = 0; i < n2; i++)
    {
    	proc->imagInOut[i] = 0;
    }

    fftInverse(&proc->tablesInverse, proc->realInOut, proc->imagInOut);

    for (int i = 0; i < n; i += 2)
    {
    	res_dbl[i] = proc->realInOut[i + 1];
    	res_dbl[i + 1] = proc->imagInOut[i + 1];
    }
}

void executeReverseTorus32(FFTProcessor *proc, APCplx res[FFTProcessor::N], const APTorus32 a[FFTProcessor::N])
{
	static const APInt64 pm33 = APInt64(1) << 33;
    static const APDouble _2pm33 = 1. / pm33;
    int n = FFTProcessor::N;
    int n2 = FFTProcessor::N2;
    APInt32 *aa = (APInt32 *)a;

    for (int i = 0; i < n; i++)
    {
    	proc->realInOut[i] = aa[i] * _2pm33;
    }

    for (int i = 0; i < n; i++)
    {
    	proc->realInOut[n + i] = -proc->realInOut[i];
    }

    for (int i = 0; i < n2; i++)
    {
    	proc->imagInOut[i] = 0;
    }

    fftInverse(&proc->tablesInverse, proc->realInOut, proc->imagInOut);

    for (int i = 0; i < FFTProcessor::Ns2; i++)
    {
    	res[i] = APCplx(proc->realInOut[2 * i + 1], proc->imagInOut[2 * i + 1]);
    }
}

void executeDirectTorus32(FFTProcessor *proc, APTorus32 res[FFTProcessor::N], const APCplx a[FFTProcessor::N])
{
    int n = FFTProcessor::N;
    int n2 = FFTProcessor::N2;
    int ns2 = FFTProcessor::Ns2;
    static const APDouble _1sN = APDouble(1) / APDouble(n);
    static const APDouble _2p32 = APDouble(APInt64(1) << 32);

    for (int i = 0; i < n; i++)
    {
    	proc->realInOut[2 * i] = 0;
    }

    for (int i = 0; i < n; i++)
    {
    	proc->imagInOut[2 * i] = 0;
    }

    for (int i = 0; i < ns2; i++)
    {
    	proc->realInOut[2 * i + 1] = a[i].real();
    }

    for (int i = 0; i < ns2; i++)
    {
    	proc->imagInOut[2 * i + 1] = a[i].imag();
    }

    for (int i = 0; i < ns2; i++)
    {
    	proc->realInOut[n2 - 1 - 2 * i] = a[i].real();
    }

    for (int i = 0; i < ns2; i++)
    {
    	proc->imagInOut[n2 - 1 - 2 * i] = -a[i].imag();
    }

    fftForward(&proc->tablesForward, proc->realInOut, proc->imagInOut);

    for (int i = 0; i < n; i++)
    {
    	res[i] = APTorus32(APInt64(proc->realInOut[i] * _1sN * _2p32));
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
