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
#include "tfhe/lagrangehalfc_arithmetic.h"

using namespace std;

// Number of HBM PCs required
#define MAX_HBM_PC_COUNT 32
#define PC_NAME(n) n | XCL_MEM_TOPOLOGY
const int pc[MAX_HBM_PC_COUNT] =
{
    PC_NAME(0),  PC_NAME(1),  PC_NAME(2),  PC_NAME(3),  PC_NAME(4),  PC_NAME(5),  PC_NAME(6),  PC_NAME(7),
    PC_NAME(8),  PC_NAME(9),  PC_NAME(10), PC_NAME(11), PC_NAME(12), PC_NAME(13), PC_NAME(14), PC_NAME(15),
    PC_NAME(16), PC_NAME(17), PC_NAME(18), PC_NAME(19), PC_NAME(20), PC_NAME(21), PC_NAME(22), PC_NAME(23),
    PC_NAME(24), PC_NAME(25), PC_NAME(26), PC_NAME(27), PC_NAME(28), PC_NAME(29), PC_NAME(30), PC_NAME(31)
};

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
	vector<cl::Platform> platforms;
	err = cl::Platform::get(&platforms);
	cl_context_properties props[3] = {CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(), 0};
	context = cl::Context(device, props, nullptr, nullptr, &err);
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

	size_t poly1Size = (size_t)(sizeof(APInt32) * PolyProcessor::N);
	size_t poly2Size = (size_t)(sizeof(APTorus32) * PolyProcessor::N);

	unsigned int dataSize = 1024;
	poly1T = std::vector<APInt32, aligned_allocator<APInt32>>(dataSize);
	poly2T = std::vector<APTorus32, aligned_allocator<APTorus32>>(dataSize);
	resultT = std::vector<APTorus32, aligned_allocator<APTorus32>>(dataSize);

    inBufExt1.obj = poly1T.data();
    inBufExt1.param = 0;
    inBufExt1.flags = pc[0];

    inBufExt2.obj = poly2T.data();
    inBufExt2.param = 0;
    inBufExt2.flags = pc[1];

    outBufExt.obj = resultT.data();
    outBufExt.param = 0;
    outBufExt.flags = pc[2];

	poly1TBuff = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR, poly1Size, &inBufExt1);
	poly2TBuff = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR, poly2Size, &inBufExt2);
	resultBuff = cl::Buffer(context, CL_MEM_WRITE_ONLY | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR, poly2Size, &outBufExt);

//	poly1T = (APInt32 *)cmdQ.enqueueMapBuffer(poly1TBuff, CL_TRUE, CL_MAP_WRITE, 0, poly1Size);
//	poly2T = (APTorus32 *)cmdQ.enqueueMapBuffer(poly2TBuff, CL_TRUE, CL_MAP_WRITE, 0, poly2Size);
//	resultT = (APTorus32 *)cmdQ.enqueueMapBuffer(resultBuff, CL_TRUE, CL_MAP_READ, 0, poly2Size);

	// set args and enqueue kernel
	int j = 0;
	kernel.setArg(j++, poly1TBuff);
	kernel.setArg(j++, poly2TBuff);
	kernel.setArg(j++, resultBuff);
}

OCLPoly::~OCLPoly()
{
//	cmdQ.enqueueUnmapMemObject(poly1TBuff, poly1T);
//	cmdQ.enqueueUnmapMemObject(poly2TBuff, poly2T);
//	cmdQ.enqueueUnmapMemObject(resultBuff, resultT);
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

void OCLPoly::polyKernel(TorusPolynomial *result, const IntPolynomial *poly1, const TorusPolynomial *poly2)
{
	for (int i = 0; i < PolyProcessor::N; i++)
	{
		poly1T[i] = poly1->coefs[i];
		poly2T[i] = poly2->coefsT[i];
	}

	// write data to DDR
	cmdQ.enqueueMigrateMemObjects({poly1TBuff, poly2TBuff}, 0);
	cmdQ.finish();

	// execute the Kernel
	cmdQ.enqueueTask(kernel);
	cmdQ.finish();

	// read data from DDR
	cmdQ.enqueueMigrateMemObjects({resultBuff}, CL_MIGRATE_MEM_OBJECT_HOST);

	// wait all to finish
	//cmdQ.flush();
	cmdQ.finish();

	for (int i = 0; i < PolyProcessor::N; i++)
	{
		result->coefsT[i] = resultT[i];
	}
}


void OCLPoly::testOp()
{
	constexpr int n = PolyProcessor::N;
	TorusPolynomial *resultK = new_TorusPolynomial(n);
	TorusPolynomial *resultC = new_TorusPolynomial(n);
	TorusPolynomial *poly2 = new_TorusPolynomial(n);
	IntPolynomial *poly1 = new_IntPolynomial_array(1, n);

	for (int i = 0; i < n; i++)
	{
		if (i % 3)
		{
			poly1->coefs[i] = 2;
			poly2->coefsT[i] = 2;
		}

		if (i % 5)
		{
			poly1->coefs[i] = 5;
			poly2->coefsT[i] = 7;
		}
		else
		{
			poly1->coefs[i] = 1;
			poly2->coefsT[i] = 1;
		}

		resultC->coefsT[i] = 0;
		resultK->coefsT[i] = 0;
	}

	torusPolynomialAddMulRFFT(resultC, poly1, poly2, true);
	torusPolynomialAddMulRFFT(resultK, poly1, poly2, false);
	int errs = 0;

	for (int i = 0; i < n; i++)
	{
		if(resultC->coefsT[i] != resultK->coefsT[i])
		{
			errs++;
//			std::cout << "Err: " << result->coefsT[i] << ",\t\t\t" << resultT[i] << endl;
		}
	}

	std::cout << "Errs: " << errs << std::endl;
}
