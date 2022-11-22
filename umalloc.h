#ifndef UMALLOC_H_
#define UMALLOC_H_

#include <stdio.h>

#define MEM_SIZE 10*1024*1024

#define HEADER_SIZE 4
#define BITS sizeof(int) * 8
#define uint unsigned int 

// void* umalloc(size_t);
// void ufree(void*);

void* umalloc(size_t bytes, char* file, int line);
void ufree(void* ptr, char* file, int line);
void printMemoryBlocks();

#define malloc(X) umalloc(X, __FILE__, __LINE__)
#define free(Y) ufree(Y, __FILE__, __LINE__)

#endif

/**
 * how to use __LINE__ and __FILE__
 * is the memory allocated for mem in umalloc contiguaous? (can check if the pointer is out of bounds for handling ufree errors?
 * */