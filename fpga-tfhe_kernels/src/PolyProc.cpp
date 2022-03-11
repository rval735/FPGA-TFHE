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

#include "PolyProc.hpp"

PolyProcessor::PolyProcessor()
{
	for (int i = 0; i < N2; i++)
	{
		realInOut[i] = 0;
	    imagInOut[i] = 0;
	}
}

void executeReverseInt(PolyProcessor proc[1],
					   APCplx res[PolyProcessor::N],
					   const APInt32 a[PolyProcessor::N])
{
	 int n = PolyProcessor::N;
	constexpr int n2 = PolyProcessor::N2;
	constexpr int ns2 = PolyProcessor::Ns2;

    for (int i = 0; i < n; i++)
    {
    	proc->realInOut[i] = a[i] / 2.;
    }

    for (int i = 0; i < n; i++)
    {
    	proc->realInOut[n + i] = -proc->realInOut[i];
    }

    for (int i = 0; i < n2; i++)
    {
    	proc->imagInOut[i] = 0;
    }

    fftInverse(proc->realInOut, proc->imagInOut);

    for (int i = 0; i < ns2; i++)
    {
    	int index = 2 * i + 1;
    	res[i].real(proc->realInOut[index]);
    	res[i].imag(proc->imagInOut[index]);
    }
}

void executeReverseTorus32(PolyProcessor proc[1],
						   APCplx res[PolyProcessor::N],
						   const APTorus32 a[PolyProcessor::N])
{
	constexpr APDouble pm33 = 1. / (APInt64(1) << 33);
	//static const double pm33 = 1.1641532182693481e-10;
	constexpr int n = PolyProcessor::N;
	constexpr int n2 = PolyProcessor::N2;

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

    fftInverse(proc->realInOut, proc->imagInOut);

    for (int i = 0; i < PolyProcessor::Ns2; i++)
    {
    	res[i] = APCplx(proc->realInOut[2 * i + 1], proc->imagInOut[2 * i + 1]);
    }
}

void executeDirectTorus32(PolyProcessor proc[1],
						  APTorus32 res[PolyProcessor::N],
						  const APCplx a[PolyProcessor::N])
{
	constexpr int n = PolyProcessor::N;
	constexpr int n2 = PolyProcessor::N2;
	constexpr int ns2 = PolyProcessor::Ns2;
//    static const double snD = 1.0 / n; // 0.0009765625;
	constexpr APDouble sN1 = 1.0 / n; //APDouble(snD);
	constexpr APDouble p32 = APDouble(APInt64(1) << 32);

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

    fftForward(proc->realInOut, proc->imagInOut);

    for (int i = 0; i < n; i++)
    {
    	APDouble partial = proc->realInOut[i] * sN1 * p32;
//    	res[i] = partial.to_ap_int();
    	res[i] = APTorus32(APInt64(partial));
    }
}

/** termwise multiplication in Lagrange space */
void lagrangeHalfCPolynomialMul(APCplx result[PolyProcessor::Ns2],
								APCplx a[PolyProcessor::Ns2],
								APCplx b[PolyProcessor::Ns2])
{
    constexpr int Ns2 = PolyProcessor::Ns2;
    for (int i = 0; i < Ns2; i++)
    {
    	result[i] = a[i] * b[i];
    }
}

// TorusPolynomial += TorusPolynomial
void torusPolynomialAddTo(APTorus32 result[PolyProcessor::N],
						  const APTorus32 b[PolyProcessor::N])
{
    constexpr int n = PolyProcessor::N;
    for (int i = 0; i < n; ++i)
    {
        result[i] = b[i];
    }
}
