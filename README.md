# Multi Threaded File Encryption - COMS 352


## Description

An implementation of a multi-threaded text file encryptor consisting of five threads and two circular buffers achieving maximum efficiency (i.e. all threads kept maximally busy).



![image](https://user-images.githubusercontent.com/57231081/179679120-1c8b81fc-1991-4805-aab6-fcda6e2fc0e0.png)

## Implementation 

#### [Main Thread](https://github.com/alexander-baumgarten/circularBuffer/blob/main/encrypt-driver.c)

-Takes in command line inputs of input file, output file, logging file, and user inputs of input/output buffer size.   
-Initializes buffers and buffer mutex locks for read and write.  
-Initialzes semaphores to signal for end of file and when to stop reading  
-Initializes five pthreads (read, readCount, encryptor, writeCount, write)  
-Joins all pthreds and terminates program  

#### [Circular Buffers](https://github.com/alexander-baumgarten/circularBuffer/blob/main/circular-buffer.c)
The input buffer holds characters received from the reader thread until processed by the encryptor thread. The reader thread is blocked if the buffer is full until a character is processed by the encryptor and input counter threads.  

The output buffer holds characters processed by the encryptor but not yet wrote out by the writer thread. If the buffer is empty, the writer thread is blocked. The buffer and writer threads continue until the EOF character is received. 

#### [Multi-threaded Encrypt Functions](https://github.com/alexander-baumgarten/circularBuffer/blob/main/encrypt-driver.c)

Five functions (read, readCount, encrypt, writeCount, write) to be used concurrently by the main module. Each function utilizes semaphore signals and mutex locks as concurrent controls to protect shared resources, namely the circular buffers.  



## Executing program


```
gcc encrypt-driver.c encrypt-module.c -lpthread -o encrypt  
```  
```
./encrypt <inputFile> <outputFile> <logFile>
```


## Authors

Contributors names and contact info
 
[@Alexander Baumgarten](https://github.com/alexander-baumgarten)  
[@Robert Rice](https://github.com/ricerob)
