# Homomorphic encryption execution on the Varium FPGA

Employ reconfigurable FPGAs to execute the Fully Homomorphic Encryption operations over the Torus (TFHE)[1], in particular the Fast Furrier Transform and Lagrange Multiplication.

### Disclaimer

To fully comprehend the terminology behind Homomorphic Encryption, it requires deep understanding of cryptography, signal processing and mathematics. Nevertheless, thanks to the high level libraries provided by Vitis, it is possible to abstract the most complicated parts of it.

The overall project might seem like a daunting task to grasp at first; however, it is possible to run the examples below without a full understanding of the inner details, even with a surprise a the end of this article.

Remember, it is not just about the destination, but about enjoying the journey to discovery.

### Background

On an every day basis everyone employs cryptography seamlessly: surfing the web using HTTPS, buying merchandise at an store using a credit card or by sending messages in (Signal)[https://signal.org/en/]. At the core, these examples take advante of the [Public Key Cryptography scheme (PKC)](https://en.wikipedia.org/wiki/Public-key_cryptography). PKC is also known as "Asymmetric Cryptography", because it employs a set of two (or more) keys to encrypt and decrypt data using a public and private key respectively. The image below shows an example of how PKC is used to transfer a message across a "unsecure" channel, which in this case is the internet.

![Public Key Cryptography scheme (PKC)](Images/PKC.png)

Blockchain implementations like Bitcoin, Ethereum or Cardano employs the PCK principle extensively in its wallet functionality, where a user creates a public address (were funds are deposited) and the private key serves to sign transactions. Something most people do not realize is all transactions in those blockchains are public. In other words, the movement of funds between public addresses is visible to all participants in the network, which makes every move traceable and possibly identifiable to individuals. That applies as well to "smart contracts" on Ethereum, Cardano or Solana, where all the transactions performed require "plain data" to operate.

A similar case occurs with public cloud services. Whenever a company uses their services (like AWS Lambda) the information transformation happens with clear data. For example, a user access a corporate site hosted on AWS, the communication happens over encrypted channels. However the user's information needs to be decrypted to have it visible to the company as well as AWS. Unless everything within the AWS servers is encrypted at every operation by the business, which in most cases is not true.

![Smart Contract Over Plain Data](Images/SmartContractOverPlainData.png)

Therefore, in order to preserve the privacy of the transactions, data transformation and verifiable results with high confidence of no leakage of information, Fully Homomorphic Encryption is the next step.

### What is Fully Homomorphic Encryption

Fully Homomorphic Encryption (FHE) is a peculiar cryptographic technique that carries out computations on encrypted data. This entitles a complete paradigm shift on data manipulation, user privacy and public services on the internet.

As alluded earlier with a generic application (ex AWS Lambda or Smart contract) receives encrypted data, first it is necessary decrypt the payload, perform the desired computations on the clear and then re-encrypt the data. On the other hand, FHE removes the need to perform decryption-encryption at once. An example of this can be seen in the image below:

[Image FHE]

In the real world, when an app needs execute a computation *F* on encrypted data, the FHE scheme proportionates an ***homomorphic*** computation *F'* that, once performed over the encrypted data, will be equivalent to the operation "decryption-encryption" required by the app over the data in the clear. In mathematical terms: F(clear_data) = Decrypt(F'(encrypted_data)).

The achievement of FHE could have a broad impact in society. It could transform the perspective on everyday computations with primordial end-to-end privacy. For example, FHE users would offload numeric-intensive calculations to cloud services with the certainty that the results would not be accessible by anyone but the private key owner. The same could be stated about public blockchains, where smart contracts execute all their operations over encrypted data without revealing the inputs-transaction-ouputs.

The mayor drawback in the FHE adoption is its slow performance. Even though there exists active research, computations over FHE encrypted data performs orders of magnitude slower than operations on plain data. In addition to that, app transformation from unencrypted data to FHE-enabled on encrypted payloads is not a trivial translation. When that translation is not properly engineered, it significantly increases the performance difference between computing on clear data and FHE encrypted data, diminishing the benefits of FHE adoption.

This [video](https://www.youtube.com/watch?v=5Mhbaeuv5fk) provides a summary of the impact FHE will have on multiple industries, like healthcare, finance or airlines. Another example [video](https://www.youtube.com/watch?v=nlsd2LO-S50) shows the possibility to use a Linear Machine Learning model to calculate a prediction over encrypted data using FHE.


FFT Explanation [video](https://www.youtube.com/watch?v=toj_IoCQE-4)

Talk about the "homomorphic" commutative diagram (screenshot)

Gentry FHE Explanation: https://www.youtube.com/watch?v=487AjvFW1lk



### References

[1 - TFHE: Fast Fully Homomorphic Encryption over the Torus](https://eprint.iacr.org/2018/421.pdf)
