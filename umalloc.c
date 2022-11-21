// user controlled memory library

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "umalloc.h"

/**
 * each block is preceded by 4 bytes of metadata.  
 * most significant bit of the 4 bytes denote whether the block is allocated or not
 * if msb is 1, block is allocated otherwise free
 * 3 bytes to store the size as an int
 * minimum size is 1 byte, hence a total of 10*2^20 bytes in 10MB, 
 * requiring 24 bits or 3 bytes in int
 * */


static char mem[MEM_SIZE];
char* head;
char* endOfMem;
char init = 'f';


/**
 * using implicit free lists 
*/

void initialize()
{
    init = 't';
    head = mem;
    // maximum available bytes in the memory, excluding 4 bytes of metadata
    uint defaultHeader = MEM_SIZE - HEADER_SIZE; 
    // printf("default header: %u\n", defaultHeader);
    *((uint*)(&mem)) = defaultHeader;
    memset(mem + HEADER_SIZE, '0', MEM_SIZE - HEADER_SIZE);
    // set head to point to the beginning of the empty block
    head += HEADER_SIZE; 
    // set endOfMem to the end of mem array
    endOfMem = mem + MEM_SIZE - 1;
}

// return the size of the block after ignoring the MSB
uint getSize(char** block)
{
    uint value = ((uint*)*block)[-1];
    value &= ~(1U << (BITS - 1));
    if (value > 0 && value < MEM_SIZE)
    {
        return value;
    } else {
        return 0;
    }
}

// check if the block is free or not by checking the msb
// returns 1 if block is allocated, 0 if free
bool isAllocated(char** block)
{
    uint value = ((uint*)*block)[-1];
    uint bit = (value >> (BITS - 1)) & 1U;
    return (bool) bit;
}

// split the empty block into used and free blocks
// ensures that the free space is kept before the used space
// returns pointer to the allocated space
char* splitAndAllocate(char** current, size_t sizeNewBlock)
{
    printf("split and allocateeeeeeeeeeeeee\n");
    uint freeSpace = getSize(current);
    // pointer to the beginnning of metadata for the new block
    char* pointerNewBlock = *current + freeSpace - sizeNewBlock - HEADER_SIZE; 
    // printf("size for block: %zu\n pointer to current: %p\t new block: %p\n", sizeNewBlock, *current, pointerNewBlock);
    uint header = (uint) sizeNewBlock;
    header |= 1 << (BITS - 1);
    // printf("header of marked block: %u\n", header);
    uint *headerPtr = (uint*)(pointerNewBlock);
    *headerPtr = header; 
    // reduce amount of free space in original block
    ((uint*)*current)[-1] = freeSpace - sizeNewBlock - HEADER_SIZE;
    pointerNewBlock += HEADER_SIZE;
    return pointerNewBlock;
}

// print all blocks in memory 
void printMemoryBlocks(char* header)
{
    printf("------------------------Memory snapshot------------------------\n");
    int i = 0;
    while (header < endOfMem && i<10)
    {
        uint size = (uint) getSize(&header);
        printf("memory: %p \tsize of block %d: %u\t allocated: %d\n",(void *) header, i, size, isAllocated(&header));
        header += size + HEADER_SIZE;
        i++;
    }
    printf("---------------------------------------------------------------\n");
}

void* umalloc(size_t bytes)
{
    void* ptr = NULL;
    char* current = head;
    if (init == 'f')
    {
        initialize();
    }
    while (current < endOfMem)
    {
        printf("in malloc: %zu\t init: %c\n", bytes, init);
        if (bytes > MEM_SIZE || bytes <= 0)
        {
            return NULL;
        }
        printMemoryBlocks(head);
        if (bytes + HEADER_SIZE < getSize(&current) && !isAllocated(&current))
        {
            printf("in malloc 1\n");
            ptr = splitAndAllocate(&current, bytes);
            if (ptr == NULL)
            {
                printf("Error allocating %lu bytes, not enough space", bytes);
            }
            break;
        }  else {
            printf("in malloc 2\n");
            current += getSize(&current);
        }
    }
    return ptr;
}

// iterate through entire memory and merge consecutive free blocks
void coalescence()
{
    char* current = head;
    uint currentSize = getSize(&current);
    char* next = current + currentSize + HEADER_SIZE;
    while (current < endOfMem)
    {
        printf("current: %p \t current size: %u \t next: %p\n", current, currentSize, next);
        if (!isAllocated(&current) && !isAllocated(&next))
        {
            uint newValue = currentSize + getSize(&next) + HEADER_SIZE;
            ((uint*)current)[-1] = newValue;
        }
        current += currentSize + HEADER_SIZE;
        currentSize = getSize(&current);
        next = current + currentSize + HEADER_SIZE;
    }
}

void ufree(void* ptr)
{
    ptr = (char*) ptr;
    uint value = ((uint*)ptr)[-1];
    value &= ~(1U << (BITS - 1));
    ((uint*)ptr)[-1] = value;
    coalescence();
    return;
}


int main() {
    initialize();
    char* ptr1 = umalloc(1000);
    // printf("ptr1: %p \t %zu\n", ptr1, sizeof(ptr1));
    char* ptr2 = umalloc(300);
    // printf("ptr2: %p \t %zu\n", ptr2, sizeof(ptr2));
    char* ptr3 = umalloc(500);
    // printf("ptr3: %p\n", ptr3);
    ufree(ptr1);
    ufree(ptr2);
    printMemoryBlocks(head);
    return 0;
}
