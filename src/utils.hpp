#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <stdio.h>
#include <netinet/in.h>



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

// time measurement macro
#define TCHK_START(name)           \
    struct timeval name##_prev;    \
    struct timeval name##_current; \
    gettimeofday(&name##_prev, NULL)

#define TCHK_END(name)                                                             \
gettimeofday(&name##_current, NULL);                                               \
time_t name##_sec;                                                                 \
suseconds_t name##_usec;                                                           \
if (name##_current.tv_sec == name##_prev.tv_sec) {                                 \
    name##_sec = name##_current.tv_sec - name##_prev.tv_sec;                       \
    name##_usec = name##_current.tv_usec - name##_prev.tv_usec;                    \
} else if (name ##_current.tv_sec != name##_prev.tv_sec) {                         \
    int name##_carry = 1000000;                                                    \
    name##_sec = name##_current.tv_sec - name##_prev.tv_sec;                       \
    if (name##_prev.tv_usec > name##_current.tv_usec) {                            \
        name##_usec = name##_carry - name##_prev.tv_usec + name##_current.tv_usec; \
        name##_sec--;                                                              \
        if (name##_usec > name##_carry) {                                          \
            name##_usec = name##_usec - name##_carry;                              \
            name##_sec++;                                                          \
        }                                                                          \
    } else {                                                                       \
        name##_usec = name##_current.tv_usec - name##_prev.tv_usec;                \
    }                                                                              \
}                                                                                  \
printf("%s: sec:%lu usec:%06d\n", #name, name##_sec, name##_usec); 

#define CTCHK_START(name)                     \
    static int name##_count = 0;              \
    static time_t name##_sec_total = 0;       \
    static suseconds_t name##_usec_total = 0; \
    struct timeval name##_prev;               \
    struct timeval name##_current;            \
    gettimeofday(&name##_prev, NULL)

#define CTCHK_END(name, count)                                                     \
gettimeofday(&name##_current, NULL);                                               \
time_t name##_sec;                                                                 \
suseconds_t name##_usec;                                                           \
if (name##_current.tv_sec == name##_prev.tv_sec) {                                 \
    name##_sec = name##_current.tv_sec - name##_prev.tv_sec;                       \
    name##_usec = name##_current.tv_usec - name##_prev.tv_usec;                    \
} else if (name ##_current.tv_sec != name##_prev.tv_sec) {                         \
    int name##_carry = 1000000;                                                    \
    name##_sec = name##_current.tv_sec - name##_prev.tv_sec;                       \
    if (name##_prev.tv_usec > name##_current.tv_usec) {                            \
        name##_usec = name##_carry - name##_prev.tv_usec + name##_current.tv_usec; \
        name##_sec--;                                                              \
        if (name##_usec > name##_carry) {                                          \
            name##_usec = name##_usec - name##_carry;                              \
            name##_sec++;                                                          \
        }                                                                          \
    } else {                                                                       \
        name##_usec = name##_current.tv_usec - name##_prev.tv_usec;                \
    }                                                                              \
}                                                                                  \
name##_sec_total += name##_sec;                                                    \
name##_usec_total += name##_usec;                                                  \
while (name##_usec_total >= 1000000) {                                             \
    name##_usec_total = name##_usec_total - 1000000;                               \
    name##_sec_total += 1;                                                         \
}                                                                                  \
name##_count++;                                                                    \
if (name##_count >= count) {                                                       \
    printf("%s: sec:%lu usec:%06d\n", #name, name##_sec_total, name##_usec_total); \
    name##_sec_total = 0;                                                          \
    name##_usec_total = 0;                                                         \
    name##_count = 0;                                                              \
}


#endif
