# Choose one of images as base image based on platform, version and OS version
FROM xilinx/xilinx_runtime_base:alveo-2021.2-ubuntu-20.04

# Copy your application and xclbin files
COPY Bitstream/fpga-tfhe /tfhe/
COPY Bitstream/PolyKernel.xclbin /tfhe/