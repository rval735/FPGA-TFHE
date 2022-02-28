# Homomorphic encryption execution on the Varium C1100 FPGA

Employ reconfigurable FPGAs to execute the [Fully Homomorphic Encryption over the Torus (TFHE)](https://eprint.iacr.org/2018/421.pdf). In particular, the Fast Furrier Transform and Lagrange Multiplication.

### Disclaimer

To fully comprehend the terminology behind Homomorphic Encryption, requires a deep understanding of cryptography, signal processing, and mathematics. Nevertheless, thanks to the high-level libraries provided by Vitis, it is possible to abstract the most complicated parts of it.

The overall project might seem like a daunting task to grasp at first; however, it is possible to run the examples below without a full understanding of the inner details, even with a surprise a the end of this article.

Remember, it is not just about the destination, but about enjoying the journey to discovery.

### Background

On an everyday basis, everyone employs cryptography seamlessly: surfing the web using HTTPS, buying merchandise at a store using a credit card, or by sending messages in (Signal)[https://signal.org/en/]. At the core, these examples take advantage of the [Public Key Cryptography scheme (PKC)](https://en.wikipedia.org/wiki/Public-key_cryptography). PKC is also known as "Asymmetric Cryptography" because it employs a set of two (or more) keys to encrypt/decrypt data using public/private keys respectively. The image below shows an example of how PKC is used to transfer a message across an "insecure" channel, which in this case is the internet.

![Public Key Cryptography scheme (PKC)](Images/PKC.png)

Blockchain implementations like Bitcoin, Ethereum, or Cardano employ the PCK principle extensively in their wallet functionality, where a user creates a public address (where funds are deposited) and the private key serves to sign transactions. Something most people do not realize is all transactions in those blockchains are public. In other words, the movement of funds between public addresses is visible to all participants in the network, which makes every move traceable and possibly identifiable to individuals. That applies as well to "smart contracts" on Ethereum, Cardano, or Solana, where all the transactions performed require "plain data" to operate.

A similar case occurs with public cloud services. Whenever a company uses its services (like AWS Lambda) the information transformation happens with clear data. For example, a user access a corporate site hosted on AWS, foreign communication happens over encrypted channels. However, the user's information needs to be decrypted to have it visible to the company as well as AWS, unless everything within the AWS servers is encrypted at every operation by the business, which in most cases is not true.

![Smart Contract Over Plain Data](Images/SmartContractOverPlainData.png)

Therefore, in order to preserve the privacy of the transactions, data transformation and verifiable results with high confidence of no leakage of important information, Fully Homomorphic Encryption is the next step.

### ¿What is Fully Homomorphic Encryption?

Fully Homomorphic Encryption (FHE) is a peculiar cryptographic technique that carries out computations on encrypted data. This entitles a complete paradigm shift on data manipulation, user privacy, and blockchain services on the public internet.

As alluded to earlier, a generic application (ex AWS Lambda or a smart contract) receives encrypted data, first, it is necessary to decrypt the payload, perform the desired computations on the clear, and then re-encrypt the data. On the other hand, FHE removes the need to perform decryption-encryption at once. An example of this can be seen in the image below:

![Computations over plain data vs FHE](Images/PlainvsFHE.png)

From the image, in the first case, all data received by the smart contract is received ciphered, then a decryption function obtains the original data, processes it in the smart contract and the results are encrypted to be sent to the recipient of that information. In the second case, thanks to FHE, the smart contract only needs to receive ciphered data, then apply all the necessary operations and output a still ciphered payload that can only be decrypted by the individual with the corresponding private key.

In the real world, when an app needs to execute a computation *F* on encrypted data, the FHE scheme proportionates an ***homomorphic*** computation *F'* that, once performed over the encrypted data, will be equivalent to the operation "decryption-encryption" required by current applications over clear data. In mathematical terms: F(clear_data) = Decrypt(F'(encrypted_data)).

The achievement of FHE could have a broad impact on society. It could transform the perspective on everyday computations with primordial end-to-end privacy. For example, FHE users would offload numeric-intensive calculations to cloud services with the certainty that the results would not be accessible by anyone but the private key owner. The same could be stated about public blockchains, where smart contracts execute all their operations over encrypted data without revealing the inputs-transaction-outputs.

The major drawback in the FHE adoption is its slow performance. Even though there is active research, computations over FHE encrypted data perform orders of magnitude slower than operations on plain data. In addition to that, app transformation from unencrypted data to FHE-enabled on encrypted payloads is not a trivial translation. When that translation is not properly engineered, it significantly increases the performance difference between computing on clear data and FHE encrypted data, diminishing the benefits of FHE adoption.

This [video](https://www.youtube.com/watch?v=5Mhbaeuv5fk) summarizes the impact FHE will have on multiple industries, like healthcare, finance, or airlines. Another example [video](https://www.youtube.com/watch?v=nlsd2LO-S50) shows the possibility of running a Linear Machine Learning model to calculate a prediction over encrypted data using FHE. This other [video](https://www.youtube.com/watch?v=487AjvFW1lk) reviews the FHE landscape.


-------



### Fully Homomorphic Encryption over the Thorus

There are multiple implementations that employ certain mathematical techniques to comply with the FHE idea. Research around this topic has been classified in 4 generations plus a Prelude stage that stablished the foundational technologies. This project focused on a particular implementation named ["Fully Homomorphic Encryption over the Torus (TFHE)"](https://tfhe.github.io/tfhe/). This was chosen because TFHE is a C/C++ framework that implements a very fast gate-by-gate bootstrapping. The open-source library evaluates arbitrary boolean circuits composed of binary gates, over encrypted data, without revealing any information on the data.

Among its capabilities, TFHE framework supports homomorphic evaluation of basic binary gates: AND, OR, XOR, NAND, NOR, NOT, MUX, XNOR. There is an important operation named ["Gate-Bootstrapping"](https://eprint.iacr.org/2010/520). This process is necessary to reduce the amount of "noise" after applying multiple TFHE operations to cyphered data. The image below provides an example of this:

![Bootstrap process in TFHE](Images/Bootstrap.png)

TFHE has no restriction on the number of gates or composition. Therefore, it can perform any computation over encrypted data, even if the applied function is not known when the data is encrypted. The library can be used with manually created circuits or using automated circuit generation tools. This [video](https://www.youtube.com/watch?v=28XlccZgiUM) explains in detail the full THFE algorithm.

### Project implementation

The focus of this project was to explore the possibility to execute functions used by the TFHE library on the Varium C1100 FPGA board. It is important to mention that TFHE has been utilized by the NuCypher Blockchain project in two implementations [here](https://github.com/nucypher/nufhe) & [here](https://github.com/nucypher/TFHE.jl). This gives this project a pespective projection into its future integration on a public blockchain.

Thanks to TFHE source code C/C++ base and Vitis HLS developer tools, it was possible to translate some processing to the FPGA, in particular operations around the (Inverse) Fast Fourier Transform (FFT) and polynomial multiplications. Derived from time constraints and source code understanding, only these operations were translated to reach the deadline with a product that employs the Varium C1100 FPGA. Future work would focus on more operations and a larger footprint of code executing on it.

With an overview of the [TFHE github](https://github.com/tfhe/tfhe) structure:

[//]: <> (brew install gource ffmpeg)
[//]: <> (gource -720x480 --seconds-per-day 0.05 --auto-skip-seconds 0.01  -o - | ffmpeg -y -r 30 -f image2pipe -vcodec ppm -i - -vcodec libx264 -preset ultrafast -pix_fmt yuv420p -crf 1 -threads 0 -bf 0 gource.mp4)

![TFHE Repository visualization](Images/Gource-TFHE.png)
https://youtu.be/aFW0Yi3MpQg

The TFHE needs to calculate a "bootstrap key" to then perform operations over encrypted data, as shown earlier. Even though key creation happens once, it is a time consuming operation. This project focused its efforts on the execution of this part into the FPGA given the need for FFT and fixed data lenghts at time of execution.

### FFT Kernel

Bootrap key creation require the following operations: 

- IFFT over integer polynomials
- IFFT over torus polynomials
- Lagrange polynomial multiplications
- FFT over torus polynomials
- Torus polynomial summation

Despite their fancy names, they involve a lot of sums/multiplications and fixed sized loop over complex and real numbers, which are excellent candidates to deploy on the Varium C1100 FPGA. Note: if you need a refresh on how to calculate a FFT, here is a [video](https://www.youtube.com/watch?v=toj_IoCQE-4) that quickly explains that it means and its importance in areas of communication and data analysis.


[Vitis HLS](https://www.xilinx.com/support/documentation/sw_manuals/xilinx2020_2/ug1399-vitis-hls.pdf) is a high-level synthesis tool that enables C/C++ to hardwire onto the device logic fabric and RAM/DSP blocks. This project takes advantage of Vitis HLS to implement hardware kernels in the [Vitis IDE](https://www.xilinx.com/products/design-tools/vitis/vitis-platform.html) flow based on the TFHE C++ code.

### Implementation 


### Results


### Docker Deployment


### Challenges

Initially, the main trouble was to define the scope of "what to work on" for the hackaton. The first approach was the development of zero knowledge proofs on the FPGA, however open source code (ex. [1](https://github.com/ZcashFoundation/zcash-fpga), [2](https://github.com/MinaProtocol/mina)) was hard to understand and difficult to translate to a working prototype.

Afterwards, another project related to blockchain arised on the developer's radar: [nucypher TFHE](https://github.com/nucypher/nufhe). Even thought the code did not compile and was not testable as is, it provided with the insights to deploy a FHE system onto the FPGA.

Fortunatelly, the TFHE repository base its code in C/C++. Thanks to Vitis HLS as a simple tool to translate code to hardware, it was possible to transfer FFT functions to a kernel that executes on the Varium C1100 with a two and a half month period.

Thanks to Hackster and Xilinx's support, it was possible to deploy this project to physical hardware. It is always important to test on it. For example, in one case, software and hardware emulations compiled the kernels, however at time of compiling the xclbin to the Varium C1100 it could not compile because of library restrictions. At the end and after many other challengues, the project reached an deliverable stage.

### Conclusion

This project is a precursor to the full deployment of TFHE circuits on reconfigurable hardware. The execution of FFT/polynomial operations only scratches the surface of the potential this line of research has for private computations in the cloud and public blockchains. 

Even though the initial results might not be impressive, the fact that Vitis HLS simplified code translation from a dynamic memory to a static-length circuit environment within the time contraints of this hackaton is an achievement on itself. 

Despite excellent support for software and hardware emulation, deployment and tests on real hardware is fundamental to align the project's objectives and its execution. 

### Future work

 This project shows how easy was to deploy C++ code to a FPGA. Next steps to take this project to the next level could involve the following:
 
 - Seek to increase the amount of functions that execute concurrently on hardware.
 - Deploy TFHE fixed circuits in hardware
 - Exploit partial reconfiguration to replace TFHE circuits on demand

 There are multiple [practical applications of FHE](https://www.youtube.com/watch?v=YLbED9OexOY), once FHE becomes more revelant in the blockchain and cloud applications, Xilinx FPGA would be well positioned to be the reference hardware in the deployment of these payloads.


### Acronyms
- FHE: Fully Homomorphic Encryption
- TFHE: FHE over the Torus
- FPGA: Field Programmable Gate Array
- CPU: Central Processing Unit
- GPU: Graphics Processing Unit
- GPGPU: General Purpose GPU
- ALU: Arithmetic Logic Unit
- DSP: Digital Signal Processor
- HBM: High Bandwidth Memory
- LUT: Look-Up Table
- OCL: Open Compute Language (OpenCL)
- CU: Compute Unit
- SW: Software
- HW: Hardware
- PCI Express (PCI-E): Peripheral Component Interconnect Express
- MUX: Multiplexor
- FFT: Fast Fourier Transform
- IDE: Integrated Development Environment

### Bio
Ph.D. rval735 is a programmer based in Auckland, New Zealand. He focuses his research on Binary Neural Networks. This project serves as the first brick to construct a bridge between blockchain technologies and Artificial Neural Networks.

He is very passionate about topics around BNN, Blockchain, and FPGA acceleration and looks forward to taking better advantage of reconfigurable hardware to make public ledger networks more efficient.

If you would like to contribute to his projects in any possible way, visit the GitHub repository, check his crypto address if you would like help with anything that you can 😉.