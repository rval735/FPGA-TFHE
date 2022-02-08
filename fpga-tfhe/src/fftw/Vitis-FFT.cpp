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
#include <algorithm>
#include <sys/time.h>
#include <cstdlib>
#include <complex>
#include <hls_stream.h>
#include <stdio.h>
#include "Vitis-FFT.h"
#include "FFTTables.hpp"
#include "fft.h"
#include "fftw/lagrangehalfc_impl.h"

using namespace std;

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
	// Get CL devices
	vector<cl::Device> devices = xcl::get_xil_devices();
	// Select first device
	device = devices[0];
	// Create context and command queue for selected device
	context = cl::Context(device, NULL, NULL, NULL, &err);
	logger.logCreateContext(err);

	cmdQ = cl::CommandQueue(context, device, OCLFLAGS, &err);

	logger.logCreateCommandQueue(err);
	string devName = device.getInfo<CL_DEVICE_NAME>();
	std::cout << "Selected Device " << devName << "\n";

	cl::Program::Binaries xclBins = xcl::import_binary_file(xclbinPath);
	devices.resize(1);
	program = cl::Program(context, devices, xclBins, NULL, &err);
	logger.logCreateProgram(err);

	kernel = cl::Kernel(program, "FFTL2Kernel", &err);
	logger.logCreateKernel(err);
	std::cout << "Kernel has been created.\n";
}

void OCLFFT::executeFFT()
{
	int n = FFTTables::FFTSize / 2;
	pair<cplx *, int32_t *> cpuLagrange = cpuFFT(n);
	pair<APCplx *, APInt32 *> fpgaLagrange = fpgaFFT(this, n);

	int err = 0;

	FFT_Processor_nayuki cpuFFT(n);
	FFTProcessor *processor = alignedAlloc<FFTProcessor>(1);

	for (int i = 0; i < n; i++)
	{
		cplx cpuFElem = cpuLagrange.first[i];
		APCplx fpgaFElem = fpgaLagrange.first[i];
		int32_t cpuSElem = cpuLagrange.second[i];
		APInt32 fpgaSElem = fpgaLagrange.second[i];

		if (real(cpuFElem) != fpgaFElem.real().to_double())
		{
			cout << "Complex diff: " << cpuFElem << " != " << fpgaFElem << endl;
			err++;
		}

		if (cpuSElem != fpgaSElem)
		{
			cout << "Int32 diff: " << cpuSElem << " != " << fpgaSElem << endl;
			err++;
		}


//		if (cpuFFT.real_inout != processor->realInOut)
//		{
//			cout << "Err proc realInOut: " << cpuFFT.real_inout << " != " << processor->realInOut << endl;
//		}
//
//		if (cpuFFT.imag_inout != double(processor->imagInOut))
//		{
//			cout << "Err proc imagInout: " << cpuFFT.imag_inout << " != " << processor->imagInOut << endl;
//		}
	}

	cout << "Errors: " << err << endl;
}

pair<cplx *, int32_t *> cpuFFT(const int &n)
{
	FFT_Processor_nayuki cpuFFT(n);
	cplx res[n];
	int32_t reals[n];

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

	cpuFFT.execute_reverse_int(res, reals);

	return {res, reals};
}

pair<APCplx *, APInt32 *> fpgaFFT(OCLFFT *oclFFT, const int &n)
{
	APCplx *res = alignedAlloc<APCplx>(n);
	APInt32 *reals = alignedAlloc<APInt32>(n);
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

	cl_mem_ext_ptr_t mextIn, mextOut, mextProc;
	mextIn = {XCL_MEM_DDR_BANK0, res, 0};
	mextOut = {XCL_MEM_DDR_BANK0, reals, 0};
	mextProc = {XCL_MEM_DDR_BANK0, processor, 0};

	cl::Buffer inBuff = cl::Buffer(oclFFT->context, CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
						(size_t)(sizeof(APCplx) * n), &mextIn);
	cl::Buffer outBuff = cl::Buffer(oclFFT->context, CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
						 (size_t)(sizeof(APUInt32) * n), &mextOut);
	cl::Buffer procBuff = cl::Buffer(oclFFT->context, CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
						  (size_t)(sizeof(processor)), &mextProc);
	oclFFT->cmdQ.finish();

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

	// write data to DDR
	vector<cl::Memory> ib;
	ib.push_back(inBuff);
	ib.push_back(procBuff);
	oclFFT->cmdQ.enqueueMigrateMemObjects(ib, 0, nullptr, &write_events[0][0]);

	// set arguments and enqueue kernel
	int j = 0;
	oclFFT->kernel.setArg(j++, inBuff);
	oclFFT->kernel.setArg(j++, inBuff);
	oclFFT->kernel.setArg(j++, procBuff);
	oclFFT->kernel.setArg(j++, outBuff);
	oclFFT->kernel.setArg(j++, outBuff);

	// Ask for kernel to execute
	oclFFT->cmdQ.enqueueTask(oclFFT->kernel, &write_events[0], &kernel_events[0][0]);

	// read data from DDR
	std::vector<cl::Memory> ob;
	ob.push_back(outBuff);
	oclFFT->cmdQ.enqueueMigrateMemObjects(ob, CL_MIGRATE_MEM_OBJECT_HOST, &kernel_events[0], &read_events[0][0]);

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

	return {res, reals};
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
