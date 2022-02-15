// Project FPGA-TFHE
// https://github.com/rval735/FPGA-TFHE
//
//                    GNU GENERAL PUBLIC LICENSE
//                       Version 3, 29 June 2007
//
// Copyright (C) 2007 Free Software Foundation, Inc. <https://fsf.org/>
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.

#include <ap_int.h>
#include <ap_fixed.h>
#include <ap_fixed_base.h>
#include <fpga/Vitis-FFT.h>
#include <algorithm>
#include <sys/time.h>
#include <cstdlib>
#include <complex>
#include <hls_stream.h>
#include <stdio.h>
#include "FFTTables.hpp"
#include "fftw/fft.h"
#include "tfhe/polynomials.h"

thread_local OCLFFT *oclKernel;

inline int tvdiff(struct timeval* tv0, struct timeval* tv1)
{
    return (tv1->tv_sec - tv0->tv_sec) * 1000000 + (tv1->tv_usec - tv0->tv_usec);
}

template <typename T>
T* alignedAlloc(size_t num)
{
    void* ptr = nullptr;
    if (posix_memalign(&ptr, 4096, num * sizeof(T))) throw bad_alloc();
    return reinterpret_cast<T*>(ptr);
}

void OCLFFT::ocl_check(const cl_int &err)
{
	if (err != CL_SUCCESS)
	{
		printf("%s:%d Error code is: %d\n", __FILE__, __LINE__, err);
	    exit(EXIT_FAILURE);
	}
}

OCLFFT::OCLFFT(string xclbinPath)
{
	cl_int err;

	// read_binary_file() is a utility API which will load the binaryFile
	// and will return the pointer to file buffer.
	std::vector<unsigned char> fileBuf = xcl::read_binary_file(xclbinPath);
    cl::Program::Binaries xclBins{{fileBuf.data(), fileBuf.size()}};

	// Get CL devices
	vector<cl::Device> devices = xcl::get_xil_devices();
	// Select first device
	device = devices[0];

	// Create OCL context for selected device
	context = cl::Context(device, nullptr, nullptr, nullptr, &err);
	logger.logCreateContext(err);

	// Create command queue for selected device
	cmdQ = cl::CommandQueue(context, device, OCLFLAGS, &err);
	logger.logCreateCommandQueue(err);


	string devName = device.getInfo<CL_DEVICE_NAME>();
	std::cout << "Selected Device " << devName << "\n";
	devices.resize(1);
	program = cl::Program(context, devices, xclBins, nullptr, &err);
	logger.logCreateProgram(err);

	kernel = cl::Kernel(program, "FFTL2Kernel", &err);
	logger.logCreateKernel(err);
	std::cout << "Kernel has been created.\n";

	poly1T = alignedAlloc<APInt32>(FFTProcessor::N);
	poly2T = alignedAlloc<APTorus32>(FFTProcessor::N);
	resultT = alignedAlloc<APTorus32>(FFTProcessor::N);

	poly1TBuff = cl::Buffer(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, (size_t)(sizeof(APInt32) * FFTProcessor::N), poly1T);
	poly2TBuff = cl::Buffer(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, (size_t)(sizeof(APTorus32) * FFTProcessor::N), poly2T);
	resultBuff = cl::Buffer(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, (size_t)(sizeof(APTorus32) * FFTProcessor::N), resultT);

	// set args and enqueue kernel
	int j = 0;
	kernel.setArg(j++, poly1TBuff);
	kernel.setArg(j++, poly2TBuff);
	kernel.setArg(j++, resultBuff);
}

void OCLFFT::torusPolynomialAddMulRFFT(TorusPolynomial *result, const IntPolynomial *poly1, const TorusPolynomial *poly2)
{
	struct timeval start_time, end_time;
	gettimeofday(&start_time, 0);

	{
		for (int i = 0; i < FFTProcessor::N; i++)
		{
			poly1T[i] = poly1->coefs[i];
			poly2T[i] = poly2->coefsT[i];
		}

		// write data to DDR
		vector<cl::Memory> ib = {poly1TBuff, poly2TBuff};
		cmdQ.enqueueMigrateMemObjects(ib, 0);

		cmdQ.enqueueTask(kernel);

		// read data from DDR
		std::vector<cl::Memory> ob = {resultBuff};
		cmdQ.enqueueMigrateMemObjects(ob, CL_MIGRATE_MEM_OBJECT_HOST);

		// wait all to finish
		//cmdQ.flush();
		cmdQ.finish();

		for (int i = 0; i < FFTProcessor::N; i++)
		{
			result->coefsT[i] = resultT[i];
		}
	}

	gettimeofday(&end_time, 0);
	std::cout << "OCL execution time " << tvdiff(&start_time, &end_time) << "us" << std::endl;

	gettimeofday(&start_time, 0);

	{
		const int32_t N = poly1->N;
		LagrangeHalfCPolynomial* tmp = new_LagrangeHalfCPolynomial_array(3,N);
		TorusPolynomial* tmpr = new_TorusPolynomial(N);
		IntPolynomial_ifft(tmp+0,poly1);
		TorusPolynomial_ifft(tmp+1,poly2);
		LagrangeHalfCPolynomialMul(tmp+2,tmp+0,tmp+1);
		TorusPolynomial_fft(tmpr, tmp+2);
		torusPolynomialAddTo(result, tmpr);
		delete_TorusPolynomial(tmpr);
		delete_LagrangeHalfCPolynomial_array(3,tmp);
	}

	gettimeofday(&end_time, 0);
	std::cout << "CPU execution time " << tvdiff(&start_time, &end_time) << "us" << std::endl;


	int err = 0;
	int n = FFTProcessor::N;

	for (int i = 0; i < n; i++)
	{
		cout << i << "\t\t" << poly1->coefs[i] << "\t\t" << poly2->coefsT[i] << std::endl;
	}

	for (int i = 0; i < n; i++)
	{
		Torus32 val0 = result->coefsT[i];
		APTorus32 val1 = resultT[i];

//		if (val0 != val1)
//		{
//			cout << "TErr #"<< i << "\t\t" << val0 << "\t\t" << val1 << endl;
//			err++;
//		}

		std::cout << i << "\t\t" << val0 << "\t\t" << val1 << std::endl;
	}

	for (int i = 0; i < n; i++)
	{
		Torus32 val0 = result->coefsT[i];
		APTorus32 val1 = resultT[i];

		if (val0 != val1)
		{
			cout << "TErr #"<< i << "\t\t" << val0 << "\t\t" << val1 << endl;
			err++;
		}
	}

	cout << "Errors: " << err << endl;
	cout << "-_- " << endl;
}

//{
//	for (int i = 0; i < FFTProcessor::N; i++)
//	{
//		poly1T[i] = poly1->coefs[i];
//		poly2T[i] = poly2->coefsT[i];
//	}
//
//	// write data to DDR
//	vector<cl::Memory> ib = {poly1TBuff, poly2TBuff};
//	cmdQ.enqueueMigrateMemObjects(ib, 0);
//
//	cmdQ.enqueueTask(kernel);
//
//	// read data from DDR
//	std::vector<cl::Memory> ob = {resultBuff};
//	cmdQ.enqueueMigrateMemObjects(ob, CL_MIGRATE_MEM_OBJECT_HOST);
//
//	// wait all to finish
//	//cmdQ.flush();
//	cmdQ.finish();
//
//	for (int i = 0; i < FFTProcessor::N; i++)
//	{
//		result->coefsT[i] = resultT[i];
//	}
//}

void OCLFFT::executeFFT()
{
	int err = 0;
	int n = FFTProcessor::N;
	const double EPSILON = 1e-8;
	TorusPolynomial *result = new TorusPolynomial(n);
	memset(result->coefsT, 0, n * sizeof(Torus32));
	IntPolynomial *poly1 = new IntPolynomial(n);
	memset(poly1->coefs, 0, n * sizeof(int32_t));
	TorusPolynomial *poly2 = new TorusPolynomial(n);
	memset(poly2->coefsT, 0, n * sizeof(Torus32));
	LagrangeHalfCPolynomial_IMPL *resIm = new LagrangeHalfCPolynomial_IMPL(n);
	memset(resIm->coefsC, 0, n * sizeof(cplx));

	APInt32 poly1T[FFTProcessor::N];
	APTorus32 poly2T[FFTProcessor::N];
	APTorus32 resultT[FFTProcessor::N];
	APCplx res[FFTProcessor::N];
	memset(res, 0, n * sizeof(APCplx));

	for (int i = 0; i < n; i++)
	{
		result->coefsT[i] = 0;
		resultT[i] = 0;

		if (i % 5)
		{
			poly1->coefs[i] = 1;
			poly1T[i] = 1;
			resIm->coefsC[i] = 1 + i;
			res[i] = 1 + i;
		}
		if (i % 7)
		{
			poly2->coefsT[i] = 1;
			poly2T[i] = 1;
		}
		else
		{
			poly1->coefs[i] = 0;
			poly2->coefsT[i] = 0;
			poly1T[i] = 0;
			poly2T[i] = 0;
			resIm->coefsC[i] = 0;
			res[i] = 0;
		}
	}


	for (int i = 0; i < n; i++)
	{
		if (poly1->coefs[i] != poly1T[i])
		{
			cout << "No poly1\t" << poly1->coefs[i] << "\t\t\t" << poly1T[i] << endl;
		}

		if (poly2->coefsT[i] != poly2T[i])
		{
			cout << "No poly2\t" << poly2->coefsT[i] << "\t\t\t" << poly2T[i] << endl;
		}
	}


	for (int i = 0; i < n; i++)
	{
		cplx resI = resIm->coefsC[i];
		double resIR = resI.real();
		double resIC = resI.imag();

		APCplx resC = res[i];
		double resCR = resC.real().to_double();
		double resCC = resC.imag().to_double();

		bool sameResR = abs(resIR - resCR) < EPSILON;
		bool sameResC = abs(resIC - resCC) < EPSILON;

		if (sameResR == false || sameResC == false)
		{
			cout << "Err #"<< i << "\t\t" << resI << "\t!=\t" << resC << endl;
			err++;
		}
	}


	struct timeval start_time, end_time;
		gettimeofday(&start_time, 0);
//	torusPolynomialAddMulRFFT(result, poly1, poly2);

	gettimeofday(&end_time, 0);
	std::cout << "Torus Total execution time " << tvdiff(&start_time, &end_time) << "us" << std::endl;

//	LagrangeHalfCPolynomial *tmp = (LagrangeHalfCPolynomial *)resIm;
//	torusPolynomialAddMulRFF(result, tmp, poly1, poly2);

	gettimeofday(&start_time, 0);
//	FFTL2Kernel(poly1T, poly2T, resultT);
	gettimeofday(&end_time, 0);
	std::cout << "Kernel Total execution time " << tvdiff(&start_time, &end_time) << "us" << std::endl;

//	FFTL2Kernel(poly1T, poly2T, resultT, res);

	for (int i = 0; i < n; i++)
	{
		Torus32 val0 = result->coefsT[i];
		APTorus32 val1 = resultT[i];

		if (val0 != val1)
		{
			cout << "TErr #"<< i << "\t\t" << val0 << "\t\t" << val1 << endl;
			err++;
		}
//		else
//		{
//			cout << i << "\t\t" << val0 << endl;
//		}

//		cplx resI = resIm->coefsC[i];
//		double resIR = resI.real();
//		double resIC = resI.imag();
//
//		APCplx resC = res[i];
//		double resCR = resC.real().to_double();
//		double resCC = resC.imag().to_double();
//
//		bool sameResR = abs(resIR - resCR) < EPSILON;
//		bool sameResC = abs(resIC - resCC) < EPSILON;
//
//		if (sameResR == false || sameResC == false)
//		{
//			cout << "Err #"<< i << "\t\t" << resI << "\t!=\t" << resC << endl;
//			err++;
//		}
	}

	cout << "Errors: " << err << endl;
}

//void OCLFFT::executeFFT()
//{
//	int n = FFTTables::FFTSize / 2;
//	tuple<vector<cplx>, vector<int32_t>, FFT_Processor_nayuki *> cpuLagrange = cpuFFT(n);
//	//pair<APCplx *, APInt32 *> fpgaLagrange = fpgaFFT(this, n);
//	tuple<vector<APCplx>, vector<APInt32>, FFTProcessor *> fpgaLagrange = kernelFFT(n);
//
//	int err = 0;
//
//	FFT_Processor_nayuki *nay = get<2>(cpuLagrange);
//	FftTables *tbd = (FftTables *)(nay->tables_direct);
//	FftTables *tbi = (FftTables *)(nay->tables_reverse);
//	FFTProcessor *fpg = get<2>(fpgaLagrange);
//	const double EPSILON = 1e-8;
//
//	for (int i = 0; i < n; i++)
//	{
//		cplx cpuFElem = get<0>(cpuLagrange)[i];
//		APCplx fpgaFElem = get<0>(fpgaLagrange)[i];
//		int32_t cpuSElem = get<1>(cpuLagrange)[i];
//		APInt32 fpgaSElem = get<1>(fpgaLagrange)[i];
//
//		double rElem = real(cpuFElem);
//		double rFlem = fpgaFElem.real().to_double();
////		double rFlem = fpgaFElem.real();
//		bool rEq = abs(rElem - rFlem) < EPSILON;
//		double cElem = imag(cpuFElem);
//		double cFlem = fpgaFElem.imag().to_double();
////		double cFlem = fpgaFElem.imag();
//		bool cEq = abs(cElem - cFlem) < EPSILON;
//
//		if (rEq == false || cEq == false)
//		{
//			cout << "Complex diff: " << cpuFElem << " != " << fpgaFElem << endl;
//			err++;
//		}
//
//		if (cpuSElem != fpgaSElem)
//		{
//			cout << "Int32 diff: " << cpuSElem << " != " << fpgaSElem << endl;
//			err++;
//		}
//
//		double rNay = nay->real_inout[i];
//		double rFpg = fpg->realInOut[i];
//		double cNay = nay->imag_inout[i];
//		double cFpg = fpg->imagInOut[i];
//		bool rEq1 = abs(rNay - rFpg) < EPSILON;
//		bool cEq1 = abs(rNay - rFpg) < EPSILON;
//
//
//		uint64_t bFNay = tbd->bit_reversed[i];
//		double tFNay = tbd->trig_tables[i];
//		APInt64 bFFpg = fpg->tablesForward.bitReversed[i];
//		double tFFpg = fpg->tablesForward.trigTables[i].to_double();
////		double tFFpg = fpg->tablesForward.trigTables[i];
//		bool bFEq = bFNay == bFFpg;
//		bool tFEq = abs(tFNay - tFFpg) < EPSILON;
//
//		uint64_t bINay = tbi->bit_reversed[i];
//		double tINay = tbi->trig_tables[i];
//		APInt64 bIFpg = fpg->tablesInverse.bitReversed[i];
//		double tIFpg = fpg->tablesInverse.trigTables[i].to_double();
////		double tIFpg = fpg->tablesInverse.trigTables[i];
//		bool bIEq = bINay == bIFpg;
//		bool tIEq = abs(tINay - tIFpg) < EPSILON;
//
//		if (rEq1 == false || cEq1 == false)
//		{
//			cout << "R&C: " << rNay << " != " << rFpg << " // " << cNay << " != " << cFpg << endl;
//			err++;
//		}
//
//		if (bFEq == false || tFEq == false || bIEq == false || tIEq == false)
//		{
//			cout << "Tab " << bFNay << " != " << bFFpg << endl;
//			cout << " \t " << tFNay << " != " << tFFpg << endl;
//			cout << " \t " << bINay << " != " << bIFpg << endl;
//			cout << " \t " << tINay << " != " << tIFpg << endl;
//			err++;
//		}
//	}
//
//	cout << "Errors: " << err << endl;
//}

tuple<vector<cplx>, vector<int32_t>, FFT_Processor_nayuki *> cpuFFT(const int &n)
{
	FFT_Processor_nayuki *cpuFFT = new FFT_Processor_nayuki(n);
	vector<cplx> res(n, 0);
	vector<int32_t> reals(n, 0);

	for (int i = 0; i < n; i++)
	{
		if (i % 5)
		{
			res[i] = 1 + i;
		}

		if (i % 7)
		{
			reals[i] = 1;
		}
	}

	for (int i = 0; i < FFTTables::FFTSize; i++)
	{
		if (i % 5)
		{
			cpuFFT->real_inout[i] = 0.5;
		}

		if (i % 7)
		{
			cpuFFT->imag_inout[i] = -0.5;
		}
	}

	cpuFFT->execute_reverse_int(res.data(), reals.data());
	cpuFFT->execute_reverse_torus32(res.data(), reals.data());
	cpuFFT->execute_direct_torus32(reals.data(), res.data());
//	fft_transform(cpuFFT->tables_direct, cpuFFT->real_inout, cpuFFT->imag_inout);
//	fft_transform_reverse(cpuFFT->tables_reverse, cpuFFT->real_inout, cpuFFT->imag_inout);

	return {res, reals, cpuFFT};
}

//tuple<vector<APCplx>, vector<APInt32>, FFTProcessor *> kernelFFT(const int &n)
//{
//	cl_int err;
//	vector<APCplx> res(n, APCplx(0));
//	vector<APInt32> reals(n, 0);
//	FFTProcessor *proc = new FFTProcessor;
//
//	for (int i = 0; i < n; i++)
//	{
//		if (i % 5)
//		{
//			res[i] = 1 + i;
//		}
//
//		if (i % 7)
//		{
//			reals[i] = 1;
//		}
//	}
//
//	for (int i = 0; i < FFTTables::FFTSize; i++)
//	{
//		if (i % 5)
//		{
//			proc->realInOut[i] = 0.5;
//		}
//
//		if (i % 7)
//		{
//			proc->imagInOut[i] = -0.5;
//		}
//	}
//
//	executeReverseInt(proc, res.data(), reals.data());
//	executeReverseTorus32(proc, res.data(), reals.data());
//	executeDirectTorus32(proc, reals.data(), res.data());
////	fftForward(&proc->tablesForward, proc->realInOut, proc->imagInOut);
////	fftInverse(&proc->tablesInverse, proc->realInOut, proc->imagInOut);
//
//	return {res, reals, proc};
//}

pair<APCplx *, APInt32 *> fpgaFFT(OCLFFT *oclFFT, const int &n)
{
	cl_int err;
	vector<APCplx, aligned_allocator<APCplx>> res(n);
	vector<APInt32, aligned_allocator<APInt32>> reals(n);
	FFTProcessor *processor = alignedAlloc<FFTProcessor>(1);

	for (int i = 0; i < n; i++)
	{
		if (i % 5)
		{
			res[i] = 1 + i;
		}

		if (i % 7)
		{
			reals[i] = 1;
		}
	}

//	cl_mem_ext_ptr_t mextIn, mextOut, mextProc;
//	mextIn = {XCL_MEM_DDR_BANK0, res, 0};
//	mextOut = {XCL_MEM_DDR_BANK0, reals, 0};
//	mextProc = {XCL_MEM_DDR_BANK0, processor, 0};

//	cl::Buffer inBuff = cl::Buffer(oclFFT->context, CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
//						(size_t)(sizeof(APCplx) * n), &mextIn);
//	cl::Buffer outBuff = cl::Buffer(oclFFT->context, CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
//						 (size_t)(sizeof(APUInt32) * n), &mextOut);
//	cl::Buffer procBuff = cl::Buffer(oclFFT->context, CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
//						  (size_t)(sizeof(processor)), &mextProc);

	cl::Buffer inBuff = cl::Buffer(oclFFT->context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
						(size_t)(sizeof(APCplx) * n), reals.data());
	cl::Buffer outBuff = cl::Buffer(oclFFT->context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
						 (size_t)(sizeof(APUInt32) * n), res.data());
	cl::Buffer procBuff = cl::Buffer(oclFFT->context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
						  (size_t)(sizeof(processor)), processor);

	std::cout << "DDR buffers have been mapped/copy-and-mapped\n";

	struct timeval start_time, end_time;
	gettimeofday(&start_time, 0);

	// Initialize events
	vector<std::vector<cl::Event> > write_events(1);
	write_events[0].resize(1);
	vector<std::vector<cl::Event> > kernel_events(1);
	kernel_events[0].resize(1);
	vector<std::vector<cl::Event> > read_events(1);
	read_events[0].resize(1);

	// set arguments and enqueue kernel
	int j = 0;
	err = oclFFT->kernel.setArg(j++, inBuff);
//	err = oclFFT->kernel.setArg(j++, inBuff);
	err = oclFFT->kernel.setArg(j++, procBuff);
//	err = oclFFT->kernel.setArg(j++, outBuff);
	err = oclFFT->kernel.setArg(j++, outBuff);
	oclFFT->logger.logCommonCheck(err, true);

	// write data to DDR
	vector<cl::Memory> ib = {inBuff, procBuff};
	//err = oclFFT->cmdQ.enqueueMigrateMemObjects(ib, 0, nullptr, &write_events[0][0]);
	err = oclFFT->cmdQ.enqueueMigrateMemObjects(ib, 0);
	oclFFT->logger.logCommonCheck(err, true);

	// Ask for kernel to execute
	//err = oclFFT->cmdQ.enqueueTask(oclFFT->kernel, &write_events[0], &kernel_events[0][0]);
	err = oclFFT->cmdQ.enqueueTask(oclFFT->kernel);
	oclFFT->logger.logCommonCheck(err, true);

	// read data from DDR
	std::vector<cl::Memory> ob = {outBuff};
	//err = oclFFT->cmdQ.enqueueMigrateMemObjects(ob, CL_MIGRATE_MEM_OBJECT_HOST, &kernel_events[0], &read_events[0][0]);
	err = oclFFT->cmdQ.enqueueMigrateMemObjects(ob, CL_MIGRATE_MEM_OBJECT_HOST);

	// wait all to finish
	oclFFT->cmdQ.flush();
	oclFFT->cmdQ.finish();

	// Query the time taken after execution
	gettimeofday(&end_time, 0);

	// profiling h2d, kernel, d2h times
	cl_ulong ts, te;
	write_events[0][0].getProfilingInfo(CL_PROFILING_COMMAND_START, &ts);
	write_events[0][0].getProfilingInfo(CL_PROFILING_COMMAND_END, &te);
	float elapsed = ((float)te - (float)ts) / 1000000.0;
	oclFFT->logger.info(xf::common::utils_sw::Logger::Message::TIME_H2D_MS, elapsed);
	kernel_events[0][0].getProfilingInfo(CL_PROFILING_COMMAND_START, &ts);
	kernel_events[0][0].getProfilingInfo(CL_PROFILING_COMMAND_END, &te);
	elapsed = ((float)te - (float)ts) / 1000000.0;
	oclFFT->logger.info(xf::common::utils_sw::Logger::Message::TIME_KERNEL_MS, elapsed);
	read_events[0][0].getProfilingInfo(CL_PROFILING_COMMAND_START, &ts);
	read_events[0][0].getProfilingInfo(CL_PROFILING_COMMAND_END, &te);
	elapsed = ((float)te - (float)ts) / 1000000.0;
	oclFFT->logger.info(xf::common::utils_sw::Logger::Message::TIME_D2H_MS, elapsed);

	// total execution time from CPU wall time
	std::cout << "Total execution time " << tvdiff(&start_time, &end_time) << "us" << std::endl;

	return {res.data(), reals.data()};
}

//	// Number of FFTs to run
//	int nffts = 1024;
//
//	ap_uint<512>* inData = alignedAlloc<ap_uint<512> >(FFT_LEN * nffts / SSR);
//	ap_uint<512>* outData = alignedAlloc<ap_uint<512> >(FFT_LEN * nffts / SSR);
//	// impulse as input
//	for (int n = 0; n < nffts; ++n)
//	{
//		for (int t = 0; t < FFT_LEN / SSR; ++t)
//		{
//			if (t == 0)
//			{
//				inData[n * FFT_LEN / SSR + t] = 1;
//			}
//			else
//			{
//				inData[n * FFT_LEN / SSR + t] = 0;
//			}
//		}
//	}
//
//	std::cout << "Host buffer has been allocated and set.\n";
//
//	cl_mem_ext_ptr_t mext_in, mext_out;
//	mext_in = {XCL_MEM_DDR_BANK0, inData, 0};
//	mext_out = {XCL_MEM_DDR_BANK0, outData, 0};
//
//	cl::Buffer in_buff;
//	in_buff = cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
//						 (size_t)(sizeof(ap_uint<512>) * (FFT_LEN * nffts / SSR)), &mext_in);
//	cl::Buffer out_buff;
//	out_buff = cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
//						  (size_t)(sizeof(ap_uint<512>) * (FFT_LEN * nffts / SSR)), &mext_out);
//	std::cout << "DDR buffers have been mapped/copy-and-mapped\n";
//
//	cmdQ.finish();
//
//
//
//	struct timeval start_time, end_time;
//	gettimeofday(&start_time, 0);
//
//	vector<std::vector<cl::Event> > write_events(1);
//	write_events[0].resize(1);
//	vector<std::vector<cl::Event> > kernel_events(1);
//	kernel_events[0].resize(1);
//	vector<std::vector<cl::Event> > read_events(1);
//	read_events[0].resize(1);
//
//	// write data to DDR
//	vector<cl::Memory> ib;
//	ib.push_back(in_buff);
//	cmdQ.enqueueMigrateMemObjects(ib, 0, nullptr, &write_events[0][0]);
//
//	// set args and enqueue kernel
//	int j = 0;
//	kernel.setArg(j++, in_buff);
//	kernel.setArg(j++, out_buff);
//	kernel.setArg(j++, nffts);
//	cmdQ.enqueueTask(kernel, &write_events[0], &kernel_events[0][0]);
//
//	// read data from DDR
//	std::vector<cl::Memory> ob;
//	ob.push_back(out_buff);
//	cmdQ.enqueueMigrateMemObjects(ob, CL_MIGRATE_MEM_OBJECT_HOST, &kernel_events[0], &read_events[0][0]);
//
//	// wait all to finish
//	//cmdQ.flush();
//	cmdQ.finish();
//	gettimeofday(&end_time, 0);
//
//	// profiling h2d, kernel, d2h times
//	cl_ulong ts, te;
//	write_events[0][0].getProfilingInfo(CL_PROFILING_COMMAND_START, &ts);
//	write_events[0][0].getProfilingInfo(CL_PROFILING_COMMAND_END, &te);
//	float elapsed = ((float)te - (float)ts) / 1000000.0;
//	logger.info(xf::common::utils_sw::Logger::Message::TIME_H2D_MS, elapsed);
//	kernel_events[0][0].getProfilingInfo(CL_PROFILING_COMMAND_START, &ts);
//	kernel_events[0][0].getProfilingInfo(CL_PROFILING_COMMAND_END, &te);
//	elapsed = ((float)te - (float)ts) / 1000000.0;
//	logger.info(xf::common::utils_sw::Logger::Message::TIME_KERNEL_MS, elapsed);
//	read_events[0][0].getProfilingInfo(CL_PROFILING_COMMAND_START, &ts);
//	read_events[0][0].getProfilingInfo(CL_PROFILING_COMMAND_END, &te);
//	elapsed = ((float)te - (float)ts) / 1000000.0;
//	logger.info(xf::common::utils_sw::Logger::Message::TIME_D2H_MS, elapsed);
//
//	// total execution time from CPU wall time
//	std::cout << "Total execution time " << tvdiff(&start_time, &end_time) << "us" << std::endl;
//
//	// check results
//	int errs = 0;
//	// step as output
//	for (int n = 0; n < nffts; ++n) {
//		for (int t = 0; t < FFT_LEN / SSR; ++t) {
//			for (int r = 0; r < SSR; r++) {
//				if (outData[n * FFT_LEN / SSR + t].range(TW_WL - 1 + TW_WL * 2 * r, TW_WL * 2 * r) != 1 ||
//					outData[n * FFT_LEN / SSR + t].range(TW_WL * 2 - 1 + TW_WL * 2 * r, TW_WL + TW_WL * 2 * r) != 0) {
//					errs++;
//					std::cout << "Real = "
//							  << outData[n * FFT_LEN / SSR + t].range(TW_WL - 1 + TW_WL * 2 * r, TW_WL * 2 * r)
//							  << "    Imag = "
//							  << outData[n * FFT_LEN / SSR + t].range(TW_WL * 2 - 1 + TW_WL * 2 * r,
//																	  TW_WL + TW_WL * 2 * r)
//							  << std::endl;
//				}
//			}
//		}
//	}
//
//	errs ? logger.error(xf::common::utils_sw::Logger::Message::TEST_FAIL)
//		 : logger.info(xf::common::utils_sw::Logger::Message::TEST_PASS);
//
//	return;
//}

void OCLFFT::executeFFTAlt()
{
}
//	cl_int err;
//	// Number of FFTs to run
//	int nffts = 32;
//	bool isForwardFFT = true;
//	int fftSSR = FFT_LEN / SSR;
//	int memSize = nffts * fftSSR;
//
//	ap_uint<512>* inData = alignedAlloc<ap_uint<512> >(memSize);
//	ap_uint<512>* midData = alignedAlloc<ap_uint<512> >(memSize);
//	ap_uint<512>* outData = alignedAlloc<ap_uint<512> >(memSize);
//
//	// impulse as input
//	for (int n = 0; n < nffts; ++n)
//	{
//		for (int t = 0; t < fftSSR; ++t)
//		{
//			if (t == 0)
//			{
//				inData[n * fftSSR + t] = 1;
//			}
//			else
//			{
//				inData[n * fftSSR + t] = 0;
//			}
//		}
//	}
//
//	std::cout << "Host buffer has been allocated and set.\n";
//
//	cl::Buffer inBuff = cl::Buffer(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
//			(size_t)(sizeof(ap_uint<512>) * memSize), inData, &err);
//	ocl_check(err);
//
//	cl::Buffer midBuff = cl::Buffer(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
//			(size_t)(sizeof(ap_uint<512>) * memSize), midData, &err);
//	ocl_check(err);
//
//	cl::Buffer outBuff = cl::Buffer(context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
//			(size_t)(sizeof(ap_uint<512>) * memSize), outData, &err);
//	ocl_check(err);
//
//	cmdQ.finish();
//	std::cout << "DDR buffers have been mapped/copy-and-mapped\n";
//
//	struct timeval start_time, end_time;
//	gettimeofday(&start_time, 0);
//
//	// set args and enqueue kernel
//	int j = 0;
//	kernel.setArg(j++, inBuff);
//	kernel.setArg(j++, midBuff);
//	kernel.setArg(j++, nffts);
//	kernel.setArg(j++, isForwardFFT);
//
//	// write data to DDR
//	err = cmdQ.enqueueMigrateMemObjects({ inBuff }, 0 /* 0 means from host*/);
//	ocl_check(err);
//
//	err = cmdQ.enqueueTask(kernel);
//	ocl_check(err);
//	//cmdQ.finish();
//
//	j = 0;
//	isForwardFFT = false;
//	kernel.setArg(j++, midBuff);
//	kernel.setArg(j++, outBuff);
//	kernel.setArg(j++, nffts);
//	kernel.setArg(j++, isForwardFFT);
//
//	err = cmdQ.enqueueTask(kernel);
//	ocl_check(err);
//	//cmdQ.finish();
//
//	// read data from DDR
//	err = cmdQ.enqueueMigrateMemObjects({ outBuff }, CL_MIGRATE_MEM_OBJECT_HOST);
//	ocl_check(err);
//	cmdQ.finish();
//
//	gettimeofday(&end_time, 0);
//
//	// total execution time from CPU wall time
//	std::cout << "Total execution time " << tvdiff(&start_time, &end_time) << "us" << std::endl;
//
//	for (int n = 0; n < nffts; ++n)
//	{
//		for (int t = 0; t < fftSSR; ++t)
//		{
//			if (inData[n * fftSSR + t] != outData[n * fftSSR + t])
//			{
//				err++;
//				//logger.error(xf::common::utils_sw::Logger::Message::TEST_FAIL);
//				cout << "n:" << n << " t: " << t << "\nIn: " << inData[n * fftSSR + t] << "\t Out: " << outData[n * fftSSR + t] << endl;
//			}
//		}
//	}
//
//	err > 0 ? logger.error(xf::common::utils_sw::Logger::Message::TEST_FAIL)
//		    : logger.info(xf::common::utils_sw::Logger::Message::TEST_PASS);
//
//
//	return;
//}
