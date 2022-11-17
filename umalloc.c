// user controlled memory library

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define MEM_SIZE 10*1024*1024

/**
 * each block is preceded by 4 bytes of metadata.  
 * most significant bit of the 4 bytes denote whether the block is allocated or not
 * if msb is 1, block is allocated otherwise free
 * 3 bytes to store the size as an int
 * minimum size is 1 byte, hence a total of 10*2^20 bytes in 10MB, 
 * requiring 24 bits or 3 bytes in int
 * */
#define HEADER_SIZE 4
#define BITS sizeof(int) * 8
#define uint unsigned int 

static char mem[MEM_SIZE];
char* freeList;
char* endOfMem;
char init = 'f';

void* umalloc(size_t bytes);
void ufree(void* ptr);

/**
 * using implicit free lists 
*/

void initialize()
{
    // memset(mem, '0', MEM_SIZE);
    init = 't';
    freeList = mem;
    // maximum available bytes in the memory, excluding 4 bytes of metadata
    uint defaultHeader = MEM_SIZE - HEADER_SIZE; 
    // uint msb = 1 << (BITS - 1);
    // printf("msb: %u\n", msb);
    // defaultHeader |= msb;
    printf("default header: %u\n", defaultHeader);
    *((uint*)(&mem)) = defaultHeader;
    memset(mem + HEADER_SIZE, '0', MEM_SIZE - HEADER_SIZE);
    // set freeList to point to the beginning of the empty block
    freeList += HEADER_SIZE; 
    // set endOfMem to the end of mem array
    endOfMem = mem + MEM_SIZE - 1;
}

// return the size of the block after ignoring the MSB
uint getSize(char* block)
{
    printf("111111\n");
    uint value = ((uint*)block)[-1];
    printf("2222222\n");
    if (value > 0 && value < MEM_SIZE)
    {
        // printf("value before shift: %u\n", value);
        value &= ~(1U << (BITS - 1)); // changed 1UL to 1U 
        return value;
    } else {
        return 0;
    }
}

// check if the block is free or not by checking the msb
bool isFree(char* block)
{
    uint value = ((uint*)block)[-1];
    uint bit = (value >> (BITS - 1)) & 1U;
    return (bool) bit;
}

// mark allocated and store the metadata in the preceding 4 bytes
void markAllocated(char* block, uint size)
{
    uint header = size;
    header |= 1 << (BITS - 1);
    printf("333333\n");
    *((uint*)(&block)) = header;    
    printf("header for allocated block: %u\n", getSize(block));
    return;
}

// split the empty block into used and free blocks
// ensures that the free space is kept before the used space
// returns pointer to the allocated space
char* splitAndAllocate(char* current, size_t sizeNewBlock)
{
    // printf("111111\n");
    uint freeSpace = getSize(current);
    // pointer to the beginnning of metadata for the new block
    char* pointerNewBlock = current + (freeSpace - sizeNewBlock) + HEADER_SIZE; 
    markAllocated(pointerNewBlock, (uint) sizeNewBlock);
    // printf("2222222\n");

    // reduce amount of free space in original block
    ((uint*)current)[-1] = freeSpace - sizeNewBlock - HEADER_SIZE;
    pointerNewBlock += HEADER_SIZE;
    return pointerNewBlock;
}

// print all blocks in memory 
void printMemoryBlocks(char* header)
{
    printf("------------------------Memory snapshot------------------------\n");
    int i = 0;
    while (header < endOfMem && i<15)
    {
        uint size = (uint) getSize(header);
        printf("size of block %d: %u\t allocated: %d\n", i, size, isFree(header));
        header += size + HEADER_SIZE;
        i++;
    }
    printf("---------------------------------------------------------------\n");
}

// void* umalloc(size_t bytes)
// {
//     void* ptr;
//     char* current = freeList;
//     if (init == 'f')
//     {
//         initialize();
//     }
//     while (current < endOfMem)
//     {
//         if (bytes < getSize(current) && isFree(current))
//         {
//             int success = splitAndAllocate(current, bytes);
//             if (!success)
//             {
//                 printf("Error allocating %lu bytes, not enough space", bytes);
//             }
//         }  else {
//             current += getSize(current);
//         }
//     }
//     return ptr;
// }

void ufree(void* ptr)
{

}

int main() {
    int i=0;
    printf("mem contents before init:\n");
    for(i=0; i<30; i++)
    {
        printf("%c", mem[i]);
    }
    printf("\n");
    initialize();
    printf("mem contents after init:\n");
    // for(i=0; i<4; i++)
    // {
    //     printf("%c", mem[i]);
    // }
    printf("get size of end: %u\n", getSize(freeList));
    printf("header: %u\n", ((uint*)mem)[0]);
    printf("isFree: %d\n", isFree(freeList));
    char* ptr = splitAndAllocate(freeList, 1000);
    printMemoryBlocks(freeList);
    // rest of mem: %s\n, freeList);
    return 0;
}