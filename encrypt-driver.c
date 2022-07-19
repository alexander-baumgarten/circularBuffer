#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include "encrypt-module.h"
#include "circular-buffer.h"

// Init all concurrency controls
struct Buffer *readBuffer; 
struct Buffer *writeBuffer; 

pthread_mutex_t readLock;
pthread_mutex_t writeLock;

pthread_t reader;
pthread_t readCounter;
pthread_t encryptor;
pthread_t writeCounter;
pthread_t writer;

sem_t endOfFile;
sem_t stopRead;

void reset_requested() {
	printf("!  Reset requested\n");
	sem_wait(&stopRead);
	while(get_input_total_count() != get_output_total_count()) { sleep(3); }
	log_counts();
	reset_finished();
}

void reset_finished() {
	printf("!  Reset finished\n");
	sem_post(&stopRead);
}

void *readT() {
	int val;
	int addVal;
	char readVal;
	while(1) {
		sem_getvalue(&endOfFile, &val);				//EOF - end execution
		//printf("^ ReadT Semaphore: %d\n", val);
		if(val == 0) { return (void*) 0; }
		sem_getvalue(&stopRead, &val);				//Reset - pause exec
		if(val == 0) { usleep(500000); }
		else {
			pthread_mutex_lock(&readLock);
			readVal = read_input();
			if(readVal == '\n') {
				pthread_mutex_unlock(&readLock);
				continue;
			}
			if(readVal == '\0' || readVal == EOF) {		//Not sure which 
				printf("!  Done with reader.\n\n");
				sem_wait(&endOfFile);
				pthread_mutex_unlock(&readLock);
				return (void*) 0;
			}
			if((addVal = add(readBuffer, readVal)) == -1) {
				printf("? Read: Add to buffer failed!\n");
				pthread_mutex_unlock(&readLock);
				usleep(250000);
			}
			else {
				printf(": Added %c at buffer index %d\n", readVal, addVal);
				pthread_mutex_unlock(&readLock);
			}
		}
	}
}

void *readCount() {
	int val;
	char countVal;
	while(1) {
		sem_getvalue(&endOfFile, &val);
		//printf("^ ReadCount Semaphore: %d\n", val);
		pthread_mutex_lock(&readLock);
		if(readBuffer->count == 0 && val == 0) { 
			printf("!  Done with read counter\n\n");
			pthread_mutex_unlock(&readLock);
			return (void*) 0; 
		}
		if((countVal = count(readBuffer)) == '\0' ) { 
			printf("? Count read: Could not read!\n"); 
			pthread_mutex_unlock(&readLock);
			usleep(250000);
		}
		else {
			count_input(countVal);		
			printf(": Counted %c to input.\n", countVal);
			pthread_mutex_unlock(&readLock);
		}
		
	}
}

void *encryptT() {
	int val;
	char toEncrypt;
	int addVal;
	while(1) {
		sem_getvalue(&endOfFile, &val);
		//printf("^ encryptT Semaphore: %d\n", val);
		pthread_mutex_lock(&readLock);
		pthread_mutex_lock(&writeLock);
		if(readBuffer->count == 0 && val == 0) {
			pthread_mutex_unlock(&readLock);
			pthread_mutex_unlock(&writeLock);
			printf("!  Done with encryptor.\n\n");
			return (void*) 0;
		}
		if(canRead(readBuffer)  == -1 || canAdd(writeBuffer) == -1 ) {
			pthread_mutex_unlock(&readLock);
			pthread_mutex_unlock(&writeLock);
			printf("? Encrypt: Couldn’t read/write.\n");
			usleep(250000);
		}
		else {
			toEncrypt = readBuf(readBuffer);
			toEncrypt = encrypt(toEncrypt);
			addVal = add(writeBuffer, toEncrypt);
			printf(": Encrypt: added %c to %d successfully\n", toEncrypt, addVal);
			pthread_mutex_unlock(&readLock);
			pthread_mutex_unlock(&writeLock);
		}
	}
}
	
void *writeCount() {
	int val;
	char countVal;
	while(1) {
		sem_getvalue(&endOfFile, &val);
		//printf("^ WriteCount Semaphore: %d\n", val);
		pthread_mutex_lock(&writeLock);
		if(writeBuffer->count == 0 && val == 0 && get_input_total_count() == get_output_total_count()) { 
			pthread_mutex_unlock(&writeLock);
			printf("!  Done with write counter\n\n");
			return (void*) 0; 
		}
		if((countVal = count(writeBuffer)) == '\0' ) {
		       //printf("? Write counter: input-total: %d, output-total: %d ? \n", get_input_total_count(), get_output_total_count());	
			printf("? Write counter: Could not read!\n"); 
			pthread_mutex_unlock(&writeLock);
			usleep(250000);
		}
		else {
			count_output(countVal);		
			printf(": Counted %c to output.\n", countVal);
			pthread_mutex_unlock(&writeLock);
		}
	}
}

void *writeT() {
int val;
char writeVal;
	while(1) {
		sem_getvalue(&endOfFile, &val);				//EOF - end execution
		//printf("^ writeT Semaphore: %d\n", val);
		pthread_mutex_lock(&writeLock);
		if(writeBuffer->count == 0 && val == 0 && get_input_total_count() == get_output_total_count()) { 
			pthread_mutex_unlock(&writeLock);
			printf("!  Done with writer\n\n");
			return (void*) 0; 
		}
		else {
			writeVal = readBuf(writeBuffer);
			if(writeVal == '\0') { 
				printf("? Reading from writeBuffer failed!\n");
				pthread_mutex_unlock(&writeLock);
				usleep(250000);
			}
			else {
				printf(": Wrote %c to output.\n", writeVal);
				write_output(writeVal);
				pthread_mutex_unlock(&writeLock);
			}
		}
	}
}


int main(int argc, char *argv[]) {
	
	if(argc != 4){ 
	     exit(-1);
	} 


	
	char *inputFilename = argv[1];
	char *outputFilename = argv[2];
	char *logFilename = argv[3];
	
	//get user buffer size 
	int m,n; 
	printf("Enter input buffer size: "); 
	scanf("%d", &m);

	printf("Enter output buffer size: "); 
	scanf("%d", &n);


	init(inputFilename, outputFilename, logFilename); 
	//Init buffers 
	readBuffer = initBuffer(m);
	writeBuffer = initBuffer(n);

	//Init mutex for buffers 
	pthread_mutex_init(&readLock, NULL); 
	pthread_mutex_init(&writeLock, NULL); 

	//init semaphores 
	sem_init(&endOfFile, 0, 1);
	sem_init(&stopRead, 0, 1);

	//pthreads for reader, input counter, encryptor, output counter, writer 
	pthread_create(&reader, NULL, &readT, NULL);
	pthread_create(&readCounter, NULL, &readCount, NULL);
	pthread_create(&encryptor, NULL, &encryptT, NULL);
	pthread_create(&writeCounter, NULL, &writeCount, NULL);
	pthread_create(&writer, NULL, &writeT, NULL);
	
	
	pthread_join(reader, NULL);
	pthread_join(readCounter, NULL);
	pthread_join(encryptor, NULL);
	pthread_join(writeCounter, NULL);
	pthread_join(writer, NULL);
	

	printf("End of file reached.\n"); 
	log_counts();
}

