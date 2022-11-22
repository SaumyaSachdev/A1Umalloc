#include "umalloc.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

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
    char* ptr1 = malloc(MEM_SIZE/4);
    printMemoryBlocks();
    char* ptr2 = malloc(MEM_SIZE/4);
    printMemoryBlocks();
    // free(ptr1);
    // free(ptr2);
    char* ptr3 = malloc(MEM_SIZE - 4);
    printMemoryBlocks();
    free(ptr3);
    printMemoryBlocks();
}

void saturationThree()
{
    int i=0;
    void* ptr;
    for (i=0; i<9216; i++)
    {
        malloc(1024);
    }
    while (ptr != NULL)
    {
        ptr = malloc(1);
    }
}

void timeOverheadFour()
{

}

int main()
{
    consistencyZero();
    maximizationOne();
    // basicCoalescenceTwo();
    printMemoryBlocks();
    return 0;
}