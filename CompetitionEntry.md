# Acceleration of homomorphic encryption with Xilinx FPGA

Employ reconfigurable FPGAs to accelerate calculations of the Fully Homomorphic Encryption over the Torus (TFHE)[1], in particular the Fast Furrier Transform.

### Disclaimer

To fully comprehend the terminology behind Homomorphic Encryption, it requires deep understanding of cryptography, signal processing and mathematics. Nevertheless, thanks to the high level libraries provided by Vitis, it is possible to abstract the most complicated parts of it.

The overall project might seem like a daunting task to grasp at first; however, it is possible to run the examples below without a full understanding of the inner details, even with a surprise a the end of this article.

Remember, it is not just about the destination, but about enjoying the journey to discovery.

### Background

On an every day basis everyone employs cryptography seamlessly, by surfing the web using HTTPS, buying merchandise at an store using a credit card or by sending messages in (Signal)[https://signal.org/en/]. At the core, these examples take advante of the [Public Key cryptography scheme](https://en.wikipedia.org/wiki/Public-key_cryptography).




### References

[1 - TFHE: Fast Fully Homomorphic Encryption over the Torus](https://eprint.iacr.org/2018/421.pdf)
