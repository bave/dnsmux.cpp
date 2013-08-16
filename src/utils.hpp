#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <stdio.h>



#define BSWAP64(IN, OUT)           \
asm volatile ("movq %1, %%rax\n\t" \
              "bswapq %%rax  \n\t" \
              "movq %%rax, %0\n\t" \
              : "=r" (OUT)         \
              : "r" (IN)           \
              : "rax"              \
              );

#define BSWAP32(IN, OUT)           \
asm volatile ("movl %1, %%eax\n\t" \
              "bswapl %%eax  \n\t" \
              "movl %%eax, %0\n\t" \
              : "=r" (OUT)         \
              : "r" (IN)           \
              : "eax"              \
              );

#define BSWAP16(IN, OUT) OUT=(uint16_t)((IN)<<8|(IN)>>8)

void
memdump(void* buffer, int length)
{
    uint32_t* addr32 = (uint32_t*)buffer;
    int i;
    int j;
    int k;
    int lines = length/16 + (length%16?1:0);
    for (i=0; i<lines; i++) {
        printf("%p : %08x %08x %08x %08x\n",
                addr32,
                htonl(*(addr32)),
                htonl(*(addr32+1)),
                htonl(*(addr32+2)),
                htonl(*(addr32+3))
        );
        addr32 += 4;
    }

    j = length%16;
    if (j == 0) return;
    k = 0;
    uint8_t*  addr8 = (uint8_t*)addr32;
    printf("%p : ", addr8);
    for (i=0; i<16; i++) {
        if (k%4 == 0 && i != 0) printf(" ");
        if (j > i) {
            printf("%02x", *addr8);
            addr8++;
        } else {
            printf("XX");
        }
        k++;
    }
    printf("\n");
    return;
}

#endif
