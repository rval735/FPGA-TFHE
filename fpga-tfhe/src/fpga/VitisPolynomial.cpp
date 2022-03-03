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
#include "fpga/VitisPolynomial.h"

OCLPoly *oclKernel;

int tvdiff(struct timeval* tv0, struct timeval* tv1)
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

void OCLPoly::ocl_check(const cl_int &err)
{
	if (err != CL_SUCCESS)
	{
		printf("%s:%d Error code is: %d\n", __FILE__, __LINE__, err);
	    exit(EXIT_FAILURE);
	}
}

OCLPoly::OCLPoly(string xclbinPath)
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

	kernel = cl::Kernel(program, "PolyKernel", &err);
	logger.logCreateKernel(err);
	std::cout << "Kernel has been created.\n";

	poly1T = alignedAlloc<APInt32>(PolyProcessor::N);
	poly2T = alignedAlloc<APTorus32>(PolyProcessor::N);
	resultT = alignedAlloc<APTorus32>(PolyProcessor::N);

	poly1TBuff = cl::Buffer(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, (size_t)(sizeof(APInt32) * PolyProcessor::N), poly1T);
	poly2TBuff = cl::Buffer(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, (size_t)(sizeof(APTorus32) * PolyProcessor::N), poly2T);
	resultBuff = cl::Buffer(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, (size_t)(sizeof(APTorus32) * PolyProcessor::N), resultT);

	// set args and enqueue kernel
	int j = 0;
	kernel.setArg(j++, poly1TBuff);
	kernel.setArg(j++, poly2TBuff);
	kernel.setArg(j++, resultBuff);
}

template<class T1, class T2>
int compareElems(T1 *elems, T2 *elems2, bool (*cmp)(T1,T2), bool errs = false, bool print = false, int n = PolyProcessor::N)
{
	int err = 0;

	if (print == true)
	{
		for (int i = 0; i < n; i++)
		{
			T1 val0 = elems[i];
			T2 val1 = elems2[i];
			cout << i << "\t\t" << val0 << "\t\t\t" << val1 << std::endl;
		}
	}

	if (errs == true)
	{
		for (int i = 0; i < n; i++)
		{
			T1 val0 = elems[i];
			T2 val1 = elems2[i];
			bool eqVal = cmp(val0, val1);

			if (eqVal == false)
			{
				cout << "TErr #"<< i << "\t\t" << val0 << "\t\t\t" << val1 << endl;
				err++;
			}
		}
	}

	return err;
}

void OCLPoly::torusPolynomialAddMulRFFT(TorusPolynomial *result, const IntPolynomial *poly1, const TorusPolynomial *poly2)
{
	for (int i = 0; i < PolyProcessor::N; i++)
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

	for (int i = 0; i < PolyProcessor::N; i++)
	{
		result->coefsT[i] = resultT[i];
	}
}
