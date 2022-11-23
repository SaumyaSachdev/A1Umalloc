// user controlled memory library
#include "umalloc.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

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
    // printf("default head: %u\n", defaultHeader);
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
    // printf("in get size, block*: %p\n", *block);
    // *block -= HEADER_SIZE;
    // printf("in get size, block* after decrement: %p\n", *block);
    uint value = ((uint*)*block)[-1];
    // printf("value: %u\n", value);
    value &= ~(1U << (BITS - 1));
    // printf("value: %u\n", value);
    if (value > 0 && value < MEM_SIZE)
    {
        // printf("value: %u\n", value);
        return value;
    } else {
        return 0;
    }
}

// check if the block is free or not by checking the msb
// returns 1 if block is allocated, 0 if free
bool isAllocated(char** block)
{
    // printf("is allocated##############\n");
    uint value = ((uint*)*block)[-1];
    // printf("value: %u\n", value);
    uint bit = (value >> (BITS - 1)) & 1U;
    // printf("bit: %u\n", bit);
    return (bool) bit;
}

// split the empty block into used and free blocks
// ensures that the free space is kept before the used space
// returns pointer to the allocated space
char* splitAndAllocate(char** current, size_t sizeNewBlock)
{
    // printf("split and allocateeeeeeeeeeeeee\n");
    uint freeSpace = getSize(current);
    // pointer to the beginnning of metadata for the new block
    // char* pointerNewBlock = *current + freeSpace - sizeNewBlock - HEADER_SIZE; 
    // printf("size for block: %zu\n pointer to current: %p\t new block: %p\n", sizeNewBlock, *current, pointerNewBlock);
    // uint head = (uint) sizeNewBlock;
    // head |= 1 << (BITS - 1);
    // printf("head of marked block: %u\n", head);
    // uint *headPtr = (uint*)(pointerNewBlock);
    // *headPtr = head; 
    // reduce amount of free space in original block
    // modify existing block metadata to contain size requested
    if (freeSpace - sizeNewBlock >= 0 && freeSpace - sizeNewBlock <= HEADER_SIZE)
    {
        uint size = (uint) sizeNewBlock;
        size |= 1 << (BITS - 1);
        ((uint*)*current)[-1] = size;
        return *current;
    } else {
        char* pointerNewBlock = *current + freeSpace - sizeNewBlock - HEADER_SIZE; 
        // printf("size for block: %zu\n pointer to current: %p\t new block: %p\n", sizeNewBlock, *current, pointerNewBlock);
        uint head = (uint) sizeNewBlock;
        head |= 1 << (BITS - 1);
        // printf("head of marked block: %u\n", head);
        uint *headPtr = (uint*)(pointerNewBlock);
        *headPtr = head; 
        uint newSize = (uint) freeSpace - sizeNewBlock - HEADER_SIZE;
        // printf("caluculation: %u\t", newSize);
        ((uint*)*current)[-1] = newSize;
        // printf("current: %u\n", ((uint*)*current)[-1]);
        pointerNewBlock += HEADER_SIZE;
        return pointerNewBlock;
    }
    
    // pointerNewBlock += HEADER_SIZE;
    // return pointerNewBlock;
}

// print all blocks in memory 
void printMemoryBlocks()
{
    printf("------------------------Memory snapshot------------------------\n");
    char* temp = head;
    // int i=0;
    while (temp < endOfMem)
    {
        uint size = (uint) getSize(&temp);
        printf("memory: %p \tsize of block: %u\t allocated: %d\n",(void *) temp, size, isAllocated(&temp));
        temp += size + HEADER_SIZE;
        // i++;
    }
    printf("---------------------------------------------------------------\n");
}

void* umalloc(size_t bytes, char* fileName, int lineNumber)
{
    void* ptr = NULL;
    int i=0;
    if (init == 'f')
    {
        initialize();
    }
    char* current = head;
    // printf("head: %p \t current: %p\n", head, current);
    while (current < endOfMem && i < 20)
    {
        // printf("in malloc: %zu\t init: %c \t head: %p\t current: %p\n", bytes, init, head, current);
        i++;
        if (bytes + HEADER_SIZE > MEM_SIZE)
        {
            printf("Error on malloc(): size requested is too large. At line %d of %s.\n", lineNumber, fileName);
            return NULL;
        }
        if (bytes <= 0)
        {
            printf("Error on malloc(): size requested is less than or equal to zero. At line %d of %s.\n", lineNumber, fileName);
            return NULL;
        }
        // printMemoryBlocks(head);
        // printf("in malloc bytes requested: %zu\tget size: %u\t is allocated: %d\n", bytes, getSize(&current), isAllocated(&current));
        if (bytes <= getSize(&current) && !isAllocated(&current))
        {
            // printf("in malloc 1----------------------------------------------------------------\n");
            ptr = splitAndAllocate(&current, bytes);
            if (ptr == NULL)
            {
                printf("Error on malloc(): cannot allocate %lu bytes, not enough space. At line %d of %s.\n", bytes, lineNumber, fileName);
                return NULL;
            }
            break;
        } else if (current + getSize(&current) < endOfMem) {
            current += getSize(&current);
        }
         else {
            // printf("in malloc 2222222222222222\n");
            printf("Error on malloc(): cannot allocate %lu bytes, not enough space. At line %d of %s.\n", bytes, lineNumber, fileName);
            return NULL;
        }
        // printf("outside if elseeeeeee\n");
    }
    return ptr;
}

// iterate through entire memory and merge consecutive free blocks
void coalescence()
{
    char* current = head;
    uint currentSize = getSize(&current);
    char* next = current + currentSize + HEADER_SIZE;
    while (next < endOfMem)
    {
        // printf("current: %p \t current size: %u \t\t next: %p\t end of mem: %p\n", current, currentSize, next, endOfMem);
        if (!isAllocated(&current) && !isAllocated(&next))
        {
            // printf("------------------------------------current: %p \t current size: %u \t next: %p\t next size: %u\n", current, currentSize, next, getSize(&next));
            
            uint newValue = currentSize + getSize(&next) + HEADER_SIZE;
            ((uint*)current)[-1] = newValue;
        } else {
            current = next;
        }
        // current += currentSize + HEADER_SIZE;
        currentSize = getSize(&current);
        next += getSize(&next) + HEADER_SIZE;
        // printf("......................................current: %p \t current size: %u \t\t next: %p\n", current, currentSize, next);

    }
    // printf("----------------------------------------------------------\n");
}

void ufree(void* pointer, char* fileName, int lineNumber)
{
    char* ptr;
    ptr = (char*) pointer;
    // printf("pointer to free: ptr: %p\t pointer: %p\n", ptr, pointer);
    if (ptr == NULL)
    {
        printf("Error on free(): trying to free a NULL pointer at line %d of %s.\n", lineNumber, fileName);
        return;
    }
    if (ptr < head || ptr > endOfMem)
    {
        printf("Error on free(): attempted to free a pointer outside of mem in line %d of %s.\n", lineNumber, fileName);
        return;
    }
    if (!isAllocated(&ptr))
    {
        printf("Error on free(): pointer already free at line %d of %s.\n", lineNumber, fileName);
        return;
    }
    uint value = ((uint*)ptr)[-1];
    value &= ~(1U << (BITS - 1));
    ((uint*)ptr)[-1] = value;
    // printf("value: %u\n", value);
    coalescence();
    // printf("end of free\n");
    return;
}


// int main() {
//     initialize();
//     char* ptr1 = malloc(2000);
//     // free(ptr1);
//     // printf("ptr1: %p \t %zu\n", ptr1, sizeof(ptr1));
//     char* ptr2 = malloc(300);
//     // printf("ptr2: %p \t %zu\n", ptr2, sizeof(ptr2));
//     // char* ptr3 = malloc(500);
//     // printf("ptr3: %p\n", ptr3);
//     printMemoryBlocks();
//     free(ptr1);
//     printMemoryBlocks();
//     free(ptr2);
//     printMemoryBlocks();
//     ptr2 = malloc(500);
//     printMemoryBlocks();
//     return 0;
// }
