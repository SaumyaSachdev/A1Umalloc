#include "umalloc.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

void consistencyZero()
{
    char* ptr1 = (char*) malloc(7);
    char* ptr1Copy = ptr1;
    memcpy(ptr1, "hello!", 7);
    free(ptr1);
    char* ptr2 = (char*) malloc(7);
    if (ptr1Copy == ptr2)
    {
        printf("SUCCESS\n");
    } else {
        printf("FAIL\n");
    }
}

void maximizationOne()
{
    uint size = 1;
    char* ptr;
    do
    {
        printf("size: %u\t ptr: %p\n", size, ptr);
        ptr = malloc(size);
        free(ptr);
        size *= 2;
    } while (ptr != NULL);
    while (ptr == NULL)
    {
        size /= 2;
        ptr = malloc(size);
    }

}

void basicCoalescenceTwo()
{
    char* ptr1 = malloc(MEM_SIZE/2);
    printMemoryBlocks();
    char* ptr2 = malloc(MEM_SIZE/4);
    printMemoryBlocks();
    free(ptr1);
    free(ptr2);
    printMemoryBlocks();
    char* ptr3 = malloc(MEM_SIZE - 4);
    printMemoryBlocks();
    free(ptr3);
    // printMemoryBlocks();
}

void saturationThree()
{
    int i=0;
    void* ptr;
    for (i=0; i<9216; i++)
    {
        ptr = malloc(1024);
        printf("allocated 1024 bytes, i: %d, ptr: %p\n", i, ptr);
    }
    printMemoryBlocks();
    while (ptr != NULL)
    {
        ptr = malloc(1);
        printf("allocated 1 byte, ptr: %p\n", ptr);
    }
    // printMemoryBlocks();
}

void timeOverheadFour()
{
    time_t beginTime, endTime;
    beginTime = time(NULL);
    endTime = time(NULL);
    printf("time elapsed: %ld\n", endTime - beginTime);
}

int main()
{
    // consistencyZero();
    // maximizationOne();
    // basicCoalescenceTwo();
    saturationThree();
    printMemoryBlocks();
    return 0;
}