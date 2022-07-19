#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "circular-buffer.h"


// Init buffer
struct Buffer* initBuffer(int sizeP){
    struct Buffer* buf = (struct Buffer*)malloc(sizeof(struct Buffer));                 // Initialize Buffer memory
    memset(buf, 0, sizeof(struct Buffer));                                              // Set everything to 0
    buf->size = sizeP;                                                                  // Set size to desired size
    buf->elements = (struct CharPos*)malloc(sizeof(struct CharPos*) * sizeP + 1);            // Initialize elements array to desired size
    memset(buf->elements, 0, sizeof(struct CharPos*) * sizeP + 1);                        // Set all chars to 0

    return buf;
}

// Frees memory
void freeBuf(struct Buffer *buf) {
    free(buf->elements);
    free(buf);
}

// Accepts a Buffer struct, returns -1 if you cannot add, 0 if adding is possible.
int canAdd(struct Buffer *buf) {
    if(buf->readIndex == buf->writeIndex && buf->count == buf->size) { return -1; }     // If indices are the same and count = size (full), we cannot add   
    else { return 0; }
}

// Accepts a Buffer struct and char, returns -1 if add wasn't possible, index of new addition if possible
int add(struct Buffer *buf, char c) {
    if(canAdd(buf) == -1) { return -1; }                                                // If we can't add, return -1
    else {
        int retIdx = buf->writeIndex;                                                   // Return index
        buf->elements[buf->writeIndex].character = c;                                   // Add element into write spot
        buf->elements[buf->writeIndex].counted = 0;
        buf->count++;                                                                   // Increase count
        buf->writeIndex++;                                                              // Increase write index
        buf->writeIndex %= buf->size;                                                   // Modulo increased count by size of buffer (overflow)
        return retIdx; 
    }
}

// Accepts a buffer and tries to read from its countIndex, if possible.
// Returns char if character can be counted, \0 if it cannot be counted.
char count(struct Buffer *buf) {
    char ret = '\0';

    if(buf->elements[buf->countIndex].character == '\0') { return ret; }       // Element is empty
    if(buf->elements[buf->countIndex].counted == 1) { return ret; }            // Preventing double counting
    else {
        ret = buf->elements[buf->countIndex].character;
        buf->elements[buf->countIndex].counted = 1;
        buf->countIndex++;
        buf->countIndex %= buf->size;
        return ret;
    }
}

int canRead(struct Buffer *buf){
    if(buf->count <= 0) { return -1; }                                                 // Buffer empty
    if(buf->elements[buf->readIndex].counted != 1) { return -1; }                      // Not counted yet
    else { return 0; }
}

// Accepts a Buffer struct, returns char popped or '\0' if not possible
char readBuf(struct Buffer *buf) {
    int start = buf->readIndex;

    if(canRead(buf) == -1) { return '\0'; }
    if(buf->count || (start % buf->size) != buf->writeIndex) {                          // Buffer not empty, count is not 0 or read pointer % size isn't the same as write
        char ret = buf->elements[buf->readIndex].character;                                  // Grab char we need to return
        buf->elements[buf->readIndex].character = '\0';                                 // Replace popped char with null terminator
        buf->elements[buf->readIndex].counted = 0;                                      // Replace popped char counted index with 0
        buf->readIndex = (buf->readIndex + 1 ) % buf->size;                             // Increment and modulo the read index
        buf->count--;                                                                   // Decrease count
	return ret;
    }
    return '\0';
}

// Helper method to test
void printHelp(struct Buffer *buf, char c, char type) {
    switch(type){
        case 'r':
            printf("Reading: readIndex: %d, writeIndex: %d, countIndex: %d, size: %d, count: %d, elements: %c (%d), %c (%d), %c (%d)\n", 
                buf->readIndex, buf->writeIndex, buf->countIndex, buf->size, buf->count, 
                buf->elements[0].character, buf->elements[0].counted, buf->elements[1].character, buf->elements[1].counted, buf->elements[2].character, buf->elements[2].counted);
            printf("Read char: %c\n\n", c);
            break;
        case 'c':
            printf("Counting: readIndex: %d, writeIndex: %d, countIndex: %d, size: %d, count: %d, elements: %c (%d), %c (%d), %c (%d)\n", 
                buf->readIndex, buf->writeIndex, buf->countIndex, buf->size, buf->count, 
                buf->elements[0].character, buf->elements[0].counted, buf->elements[1].character, buf->elements[1].counted, buf->elements[2].character, buf->elements[2].counted);
            printf("Counted char: %c\n\n", c);
            break;
    }
}
/*
int main(int argc, char* argv[]) {
    struct Buffer *buf = initBuffer(3);
    char readF;
    int idx;
    char countF;

    printf("Nothing added: readIndex: %d, writeIndex: %d, size: %d, count: %d, elements: %c, %c, %c\n", 
    buf->readIndex, buf->writeIndex, buf->size, buf->count, buf->elements[0].character, buf->elements[1].character, buf->elements[2].character);
    
    idx = add(buf, 'u');
    printf("Adding u: readIndex: %d, writeIndex: %d, countIndex: %d, size: %d, count: %d, elements: %c (%d), %c (%d), %c (%d)\n", 
            buf->readIndex, buf->writeIndex, buf->countIndex, buf->size, buf->count, 
            buf->elements[0].character, buf->elements[0].counted, buf->elements[1].character, buf->elements[1].counted, buf->elements[2].character, buf->elements[2].counted);
    printf("Added index: %d\n\n", idx);

    idx = add(buf, 'v');
    printf("Adding v: readIndex: %d, writeIndex: %d, countIndex: %d, size: %d, count: %d, elements: %c (%d), %c (%d), %c (%d)\n", 
            buf->readIndex, buf->writeIndex, buf->countIndex, buf->size, buf->count, 
            buf->elements[0].character, buf->elements[0].counted, buf->elements[1].character, buf->elements[1].counted, buf->elements[2].character, buf->elements[2].counted);
    printf("Added index: %d\n\n", idx);

    idx = add(buf, 'w');
    printf("Adding w: readIndex: %d, writeIndex: %d, countIndex: %d, size: %d, count: %d, elements: %c (%d), %c (%d), %c (%d)\n", 
            buf->readIndex, buf->writeIndex, buf->countIndex, buf->size, buf->count, 
            buf->elements[0].character, buf->elements[0].counted, buf->elements[1].character, buf->elements[1].counted, buf->elements[2].character, buf->elements[2].counted);
    printf("Added index: %d\n\n", idx);

    idx = add(buf, 'f');
    printf("Trying to add f: readIndex: %d, writeIndex: %d, size: %d, count: %d, elements: %c, %c, %c\n", 
    buf->readIndex, buf->writeIndex, buf->size, buf->count, buf->elements[0].character, buf->elements[1].character, buf->elements[2].character);
    printf("Added index: %d\n\n", idx);
    
    idx = add(buf, 'f');
    printf("Trying to add f again: readIndex: %d, writeIndex: %d, size: %d, count: %d, elements: %c, %c, %c\n", 
    buf->readIndex, buf->writeIndex, buf->size, buf->count, buf->elements[0].character, buf->elements[1].character, buf->elements[2].character);
    printf("Added index: %d\n\n", idx);
    
    countF = count(buf);
    printHelp(buf, countF, 'c');

    readF = read(buf);
    printHelp(buf, readF, 'r');

    countF = count(buf);
    printHelp(buf, countF, 'c');

    readF = read(buf);
    printHelp(buf, readF, 'r');

    readF = read(buf);
    printHelp(buf, readF, 'r');

    idx = add(buf, 'f');
    printf("Adding f: readIndex: %d, writeIndex: %d, countIndex: %d, size: %d, count: %d, elements: %c (%d), %c (%d), %c (%d)\n", 
            buf->readIndex, buf->writeIndex, buf->countIndex, buf->size, buf->count, 
            buf->elements[0].character, buf->elements[0].counted, buf->elements[1].character, buf->elements[1].counted, buf->elements[2].character, buf->elements[2].counted);
    printf("Added index: %d\n\n", idx);
	
    freeBuf(buf);
}*/
