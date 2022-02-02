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
#include <cassert>
#include <cmath>

#include "FFTProc.hpp"
//#include "tfhe/polynomials.h"
//#include "lagrangehalfc_impl.h"
//#include "fft.h"

FFTProcessor::FFTProcessor()
{
    for (int32_t x=0; x<_2N; x++)
    {
    	omegaxminus1[x] = cplx(cos(x*M_PI/N)-1., sin(x*M_PI/N));
    }
}

void FFTProcessor::executeReverseInt(cplx *res, const int32_t *a)
{
    double *res_dbl=(double *)res;
    for (int32_t i = 0; i < N; i++)
    {
    	realInOut[i] = a[i]/2.;
    }

    for (int32_t i = 0; i < N; i++)
    {
    	realInOut[N + i] =-realInOut[i];
    }

    for (int32_t i = 0; i <_2N; i++)
    {
    	imagInOut[i] = 0;
    }

    FFTTables::fftInverse(tablesInverse, realInOut, imagInOut);

    for (int32_t i = 0; i < N; i += 2)
    {
    	res_dbl[i] = realInOut[i + 1];
    	res_dbl[i + 1] = imagInOut[i + 1];
    }
}

void FFTProcessor::executeReverseTorus32(cplx *res, const Torus32 *a)
{
    static const double _2pm33 = 1./double(INT64_C(1)<<33);
    int32_t *aa = (int32_t *)a;

    for (int32_t i = 0; i < N; i++)
    {
    	realInOut[i] = aa[i] * _2pm33;
    }

    for (int32_t i = 0; i < N; i++)
    {
    	realInOut[N + i] = -realInOut[i];
    }

    for (int32_t i = 0; i < _2N; i++)
    {
    	imagInOut[i] = 0;
    }

    FFTTables::fftInverse(tablesInverse, realInOut, imagInOut);

    for (int32_t i = 0; i < Ns2; i++)
    {
    	res[i] = cplx(realInOut[2 * i + 1], imagInOut[2 * i + 1]);
    }
}

void FFTProcessor::executeDirectTorus32(Torus32 *res, const cplx *a)
{
    static const double _2p32 = double(INT64_C(1)<<32);
    static const double _1sN = double(1)/double(N);

    for (int32_t i = 0; i < N; i++)
    {
    	realInOut[2 * i] = 0;
    }

    for (int32_t i = 0; i < N; i++)
    {
    	imagInOut[2 * i] = 0;
    }

    for (int32_t i = 0; i < Ns2; i++)
    {
    	realInOut[2 * i + 1] = real(a[i]);
    }

    for (int32_t i = 0; i < Ns2; i++)
    {
    	imagInOut[2 * i + 1] = imag(a[i]);
    }

    for (int32_t i = 0; i < Ns2; i++)
    {
    	realInOut[_2N - 1 - 2 * i] = real(a[i]);
    }

    for (int32_t i = 0; i < Ns2; i++)
    {
    	imagInOut[_2N - 1 - 2 * i] = -imag(a[i]);
    }

    FFTTables::fftForward(tablesForward, realInOut, imagInOut);

    for (int32_t i = 0; i < N; i++)
    {
    	res[i] = Torus32(int64_t(realInOut[i] * _1sN * _2p32));
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
