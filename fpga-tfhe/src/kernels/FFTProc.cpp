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

void executeReverseInt(FFTProcessor proc[1], APCplx res[FFTProcessor::N], const APInt32 a[FFTProcessor::N])
{
	constexpr int n = FFTProcessor::N;
	constexpr int n2 = FFTProcessor::N2;
//	APDouble *res_dbl = (APDouble *)res;

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
    	res[i] = APCplx(proc->realInOut[i], proc->imagInOut[i]);
//    	res_dbl[i] = proc->realInOut[i + 1];
//    	res_dbl[i + 1] = proc->imagInOut[i + 1];
    }
}

void executeReverseTorus32(FFTProcessor proc[1], APCplx res[FFTProcessor::N], const APTorus32 a[FFTProcessor::N])
{
	static const APDouble pm33 = 1. / (APInt64(1) << 33);
	//static const double pm33 = 1.1641532182693481e-10;
    int n = FFTProcessor::N;
    int n2 = FFTProcessor::N2;

    for (int i = 0; i < n; i++)
    {
    	proc->realInOut[i] = a[i] * pm33;
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

void executeDirectTorus32(FFTProcessor proc[1], APTorus32 res[FFTProcessor::N], const APCplx a[FFTProcessor::N])
{
    int n = FFTProcessor::N;
    int n2 = FFTProcessor::N2;
    int ns2 = FFTProcessor::Ns2;
//    static const double snD = 1.0 / n; // 0.0009765625;
    static const APDouble sN1 = 1.0 / n; //APDouble(snD);
    static const APDouble p32 = APDouble(APInt64(1) << 32);

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

//    std::cout <<"Rio\t\tpartial\n";
    for (int i = 0; i < n; i++)
    {
    	APDouble partial = proc->realInOut[i] * sN1 * p32;
    	res[i] = partial.to_ap_int();
//    	std::cout << std::setprecision (12) << res[i] << "\t\t" << partial << std::endl;
    }
}

/** termwise multiplication in Lagrange space */
void lagrangeHalfCPolynomialMul(APCplx result[FFTProcessor::Ns2],
								APCplx a[FFTProcessor::Ns2],
								APCplx b[FFTProcessor::Ns2])
{
    constexpr int Ns2 = FFTProcessor::Ns2;
    for (int i = 0; i < Ns2; i++)
    {
    	result[i] = a[i] * b[i];
    }
}

// TorusPolynomial += TorusPolynomial
void torusPolynomialAddTo(APTorus32 result[FFTProcessor::N], const APTorus32 b[FFTProcessor::N])
{
    constexpr int n = FFTProcessor::N;
    for (int i = 0; i < n; ++i)
    {
        result[i] += b[i];
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
