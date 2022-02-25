# Homomorphic encryption execution on the Varium FPGA

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

This [video](https://www.youtube.com/watch?v=5Mhbaeuv5fk) provides a summary of the impact FHE will have on multiple industries, like healthcare, finance, or airlines. Another example [video](https://www.youtube.com/watch?v=nlsd2LO-S50) shows the possibility to use a Linear Machine Learning model to calculate a prediction over encrypted data using FHE.


FFT Explanation [video](https://www.youtube.com/watch?v=toj_IoCQE-4)

Talk about the "homomorphic" commutative diagram (screenshot)

Gentry FHE Explanation: https://www.youtube.com/watch?v=487AjvFW1lk




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

### Bio
Ph.D. rval735 is a programmer based in Auckland, New Zealand. He focuses his research on Binary Neural Networks. This project serves as the first brick to construct a bridge between blockchain technologies and Artificial Neural Networks.

He is very passionate about topics around BNN, Blockchain, and FPGA acceleration and looks forward to taking better advantage of reconfigurable hardware to make public ledger networks more efficient.

If you would like to contribute to his projects in any possible way, visit the GitHub repository, check his crypto address if you would like help with anything that you can 😉.