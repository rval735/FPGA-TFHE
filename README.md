# Acceleration of homomorphic encryption with Xilinx FPGA

This project takes advantage of Xilinx reconfigurable FPGAs to accelerate calculations of the Fully Homomorphic Encryption over the Torus (TFHE), in particular the Fast Furrier Transform (FFT).

This development is part of the [Adaptive Computing Developer Contest 2021 by Xilinx](https://www.hackster.io/contests/xilinxadaptivecomputing2021/). The entry is explained in this [link.](https://www.hackster.io/rval735/acceleration-of-homomorphic-encryption-with-xilinx-fpga-24d550) as well as the file "CompetitionEntry.md" within this repository

### TL;DR

Use the Xilinx Varium C1100 to perform the FFT computation employed by the TFHE to accelerate the number processing elements compared to typical CPUs. The image below provides a high-level overview of the system architecture.

### Code structure

### Requirements

The author used a desktop computer with i5 6th gen, 16GB Ram and 1TB SSD to connect to the Varium C1100 FPGA provided by Xilinx. It is possible to execute this project on a Alveo U50 as well.

[Vitis 2021.2 IDE](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/vitis.html) and the [Vitis DSP Library](https://xilinx.github.io/Vitis_Libraries/dsp/2020.1/index.html) were employed to deploy preconfigured bitstreams to the FPGA.

### Install

### Experiments

### Docker Deployment

### License

This project bootstrapped from the original TFHE implementation found [here](https://github.com/tfhe/tfhe). That code uses Apache 2 License, while new additions in this project employ the license GPL 3. If there are doubts about it, check on the top of each file to find the corresponding license.
