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
#include <algorithm>
#include <sys/time.h>
#include <cstdlib>
#include <complex>
#include <hls_stream.h>
#include <vt_fft.hpp>
#include <data_path.hpp>


#include "Vitis-FFT.h"

using namespace std;

void fft_forward(hls::stream<T_in> p_inData[SSR], hls::stream<T_out_F> p_outData[SSR])
{
    xf::dsp::fft::fft<fftForward, IID>(p_inData, p_outData);
}

void fft_reverse(hls::stream<T_out_F> p_inData[SSR], hls::stream<T_out_I> p_outData[SSR])
{
    xf::dsp::fft::fft<fftInverse, IID>(p_inData, p_outData);
}


inline int tvdiff(struct timeval* tv0, struct timeval* tv1) {
    return (tv1->tv_sec - tv0->tv_sec) * 1000000 + (tv1->tv_usec - tv0->tv_usec);
}

template <typename T>
T* alignedAlloc(size_t num)
{
    void* ptr = nullptr;
    if (posix_memalign(&ptr, 4096, num * sizeof(T))) throw bad_alloc();
    return reinterpret_cast<T*>(ptr);
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

	kernel = cl::Kernel(program, "fft1DKernel", &err);
	logger.logCreateKernel(err);
	std::cout << "Kernel has been created.\n";

}

void OCLFFT::executeFFT()
{
	// Number of FFTs to run
	int nffts = 3;

	ap_uint<512>* inData = alignedAlloc<ap_uint<512> >(FFT_LEN * nffts / SSR);
	ap_uint<512>* outData = alignedAlloc<ap_uint<512> >(FFT_LEN * nffts / SSR);
	// impulse as input
	for (int n = 0; n < nffts; ++n) {
		for (int t = 0; t < FFT_LEN / SSR; ++t) {
			if (t == 0)
				inData[n * FFT_LEN / SSR + t] = 1;
			else
				inData[n * FFT_LEN / SSR + t] = 0;
		}
	}

	std::cout << "Host buffer has been allocated and set.\n";

	cl_mem_ext_ptr_t mext_in, mext_out;
	mext_in = {XCL_MEM_DDR_BANK0, inData, 0};
	mext_out = {XCL_MEM_DDR_BANK0, outData, 0};

	cl::Buffer in_buff;
	in_buff = cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
						 (size_t)(sizeof(ap_uint<512>) * (FFT_LEN * nffts / SSR)), &mext_in);
	cl::Buffer out_buff;
	out_buff = cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
						  (size_t)(sizeof(ap_uint<512>) * (FFT_LEN * nffts / SSR)), &mext_out);
	std::cout << "DDR buffers have been mapped/copy-and-mapped\n";

	cmdQ.finish();



	struct timeval start_time, end_time;
	gettimeofday(&start_time, 0);

	vector<std::vector<cl::Event> > write_events(1);
	write_events[0].resize(1);
	vector<std::vector<cl::Event> > kernel_events(1);
	kernel_events[0].resize(1);
	vector<std::vector<cl::Event> > read_events(1);
	read_events[0].resize(1);

	// write data to DDR
	vector<cl::Memory> ib;
	ib.push_back(in_buff);
	cmdQ.enqueueMigrateMemObjects(ib, 0, nullptr, &write_events[0][0]);

	// set args and enqueue kernel
	int j = 0;
	kernel.setArg(j++, in_buff);
	kernel.setArg(j++, out_buff);
	kernel.setArg(j++, nffts);
	cmdQ.enqueueTask(kernel, &write_events[0], &kernel_events[0][0]);

	// read data from DDR
	std::vector<cl::Memory> ob;
	ob.push_back(out_buff);
	cmdQ.enqueueMigrateMemObjects(ob, CL_MIGRATE_MEM_OBJECT_HOST, &kernel_events[0], &read_events[0][0]);

	// wait all to finish
	cmdQ.flush();
	cmdQ.finish();
	gettimeofday(&end_time, 0);

	// profiling h2d, kernel, d2h times
	cl_ulong ts, te;
	write_events[0][0].getProfilingInfo(CL_PROFILING_COMMAND_START, &ts);
	write_events[0][0].getProfilingInfo(CL_PROFILING_COMMAND_END, &te);
	float elapsed = ((float)te - (float)ts) / 1000000.0;
	logger.info(xf::common::utils_sw::Logger::Message::TIME_H2D_MS, elapsed);
	kernel_events[0][0].getProfilingInfo(CL_PROFILING_COMMAND_START, &ts);
	kernel_events[0][0].getProfilingInfo(CL_PROFILING_COMMAND_END, &te);
	elapsed = ((float)te - (float)ts) / 1000000.0;
	logger.info(xf::common::utils_sw::Logger::Message::TIME_KERNEL_MS, elapsed);
	read_events[0][0].getProfilingInfo(CL_PROFILING_COMMAND_START, &ts);
	read_events[0][0].getProfilingInfo(CL_PROFILING_COMMAND_END, &te);
	elapsed = ((float)te - (float)ts) / 1000000.0;
	logger.info(xf::common::utils_sw::Logger::Message::TIME_D2H_MS, elapsed);

	// total execution time from CPU wall time
	std::cout << "Total execution time " << tvdiff(&start_time, &end_time) << "us" << std::endl;

	// check results
	int errs = 0;
	// step as output
	for (int n = 0; n < nffts; ++n) {
		for (int t = 0; t < FFT_LEN / SSR; ++t) {
			for (int r = 0; r < SSR; r++) {
				if (outData[n * FFT_LEN / SSR + t].range(TW_WL - 1 + TW_WL * 2 * r, TW_WL * 2 * r) != 1 ||
					outData[n * FFT_LEN / SSR + t].range(TW_WL * 2 - 1 + TW_WL * 2 * r, TW_WL + TW_WL * 2 * r) != 0) {
					errs++;
					std::cout << "Real = "
							  << outData[n * FFT_LEN / SSR + t].range(TW_WL - 1 + TW_WL * 2 * r, TW_WL * 2 * r)
							  << "    Imag = "
							  << outData[n * FFT_LEN / SSR + t].range(TW_WL * 2 - 1 + TW_WL * 2 * r,
																	  TW_WL + TW_WL * 2 * r)
							  << std::endl;
				}
			}
		}
	}

	errs ? logger.error(xf::common::utils_sw::Logger::Message::TEST_FAIL)
		 : logger.info(xf::common::utils_sw::Logger::Message::TEST_PASS);

	return;
}
