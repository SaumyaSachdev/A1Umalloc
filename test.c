#include <stdio.h>

void toHex(unsigned int num, char* hex)
{
    sprintf(hex, "%x", num);
    
}


int main()
{
    char mem[] = {'a','b','c','d','e','f','g'};
    unsigned int size = 130;
    int i=0;
    char hex[6] = {0};
    toHex(size, hex);
    printf("hex representation: \n");
    for (i=0; i<6; i++)
    {
        printf("%c", hex[i]);
    }
    printf("\n");
    mem[3] = 0b1000000;
    char* ptr = mem;
    // for (i=0;i<7;i++) 
    // {
    //     printf("%c\n", mem[i]);
    // }
    printf("fourth element: %u\n", mem[3]);
    printf("unsigned int size: %lu\n", sizeof(unsigned int));
    // printf("first element %s \n", ptr);
    // printf("first element address %p \n", ptr);
    // printf("second element address %p \n", ptr+1);
    
    return 0;
}