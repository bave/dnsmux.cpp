#ifndef __NAME_PKT_H
#define __NAME_PKT_H

/*
 * 0                16               31
 * +----------------+----------------+
 * |   identifier   |     flag       |
 * +----------------+----------------+
 * |   Ques Number  |   Ans Number   |
 * +----------------+----------------+
 * |    NS Number   |   Add Number   |
 * +----------------+----------------+
 * :                                 :
 * :    Question Resource Record     :
 * :                                 :
 * +----------------+----------------+
 * :                                 :
 * :     Answer Resource Record      :
 * :                                 :
 * +----------------+----------------+
 * :                                 :
 * :   NameServer Resource Record    :
 * :                                 :
 * +----------------+----------------+
 * :                                 :
 * :    Additonal Resource Record    :
 * :                                 :
 * +----------------+----------------+
 *
 *
 * identifier: You set Question == Reply.
 *
 * flags:
 * 0    1                5    6    7    8     9          11               15
 * +----+----------------+----+----+----+----+-----------+----------------+
 * | QR | Operation Code | AA | TC | RD | RA |   Zero    |    Recode      |
 * +----+----------------+----+----+----+----+-----------+----------------+
 *
 * QR : Question or Reply 
 *  0 : Question
 *  1 : Reply
 *
 * Operation Code
 * 0000 : standard
 * 0001 : reverse request
 * 0011 : server status request
 *
 * AA : Authoritative Answer
 *  0 : no
 *  1 : yes
 *
 * TC : Truncated
 * (UDP segments over 512bytes, Only the first 512bytes of the reply)
 *  0 : no
 *  1 : yes
 *
 * RD : Recursion Desired
 *  0 : no
 *  1 : yes
 *
 * RA : recursion available
 *  0 : no
 *  1 : yes
 *
 * Zero : There is a 3bits filling by Zero.
 *
 * Recode : return code 
 * 0000 : no error
 * 0011 : name error
 *
 *
 * Question Resource Record type=A
 * 0                16               31
 * +----------------+----------------+
 * |           Query Name            |
 * +----------------+----------------+
 * |   Query Type   | Protocol Class |
 * +----------------+----------------+
 * 
 * Query Type : 
 *  1 // A
 *  5 // CNAME
 * 12 // PTR
 * 13 // HINFO
 * 14 // MINFO
 * 15 // MX
 * 28 // AAAA
 * 55 // ANY (ALL TYPE REQUEST)
 *
 * Protocol Class :
 * 1 // internet
 *
 * Answer Resource Record
 * NameServer Resource Record
 * Additonal Resource Record
 * 0                16               31
 * +----------------+----------------+
 * :           Query Name            :
 * +----------------+----------------+
 * |   Query TYPE   | Protocol Class |
 * +----------------+----------------+
 * |          Time To Live           |
 * +----------------+----------------+
 * |  rdata_length  |                :
 * +----------------+                +
 * :           Resource Data         :
 * +----------------+----------------+
 *
 * Time To Live : client cashe time [sec]
 *
 * Resource Date length:
 * Query type    1 : 4byte
 *              12 : XX byte
 *              15 : 4byte
 *              28 : 16byte
 *
 */


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <iostream>
#include <vector>
#include <string>

using namespace std;

class name_pkt {
#define DNS_MAX_LEN_SIZE 64
#define UDP_PACKET_SIZE 512

// uint16_t buf , u_char *pointer
#define N_READ16(buf, cp) {   \
    memcpy(&(buf), (cp), 2);  \
    for(size_t i=0; i<2; i++) \
    cp++;                     \
}

// uint32_t buf , u_char *pointer
#define N_READ32(buf, cp) {   \
    memcpy(&buf, cp, 4);      \
    for(size_t i=0; i<4; i++) \
    cp++;                     \
}

// uint16_t buf , u_char *pointer
#define N_WRITE16(buf, cp) {  \
    memcpy(cp,&buf,2);        \
    for(size_t i=0; i<2; i++) \
    cp++;                     \
                              \
}

// uint32_t buf , u_char *pointer
#define N_WRITE32(buf, cp) {  \
    memcpy(cp,&buf,4);        \
    for(size_t i=0; i<4; i++) \
    cp++;                     \
}

#define N_WRITE128(buf, cp) {  \
    memcpy(cp,&buf,16);        \
    for(size_t i=0; i<16; i++) \
    cp++;                      \
}

public:

// struct member
struct _rr_pkt {
    char rr_name[DNS_MAX_LEN_SIZE];
    size_t rr_size;
    uint16_t rr_type;
    uint16_t rr_class;
    uint32_t rr_ttl;
    uint16_t rr_rlen;
    union {
        char rr_rdata[DNS_MAX_LEN_SIZE];
        union {
            struct in_addr rr_sin_addr;
            struct in6_addr rr_sin6_addr;
        };
    };
};

// function
name_pkt();
~name_pkt();

u_char *n_payload(void);
size_t n_payload_size(void);

void n_init(void);
void n_set_id(int i);
void n_reset_flags(void);
void n_set_flags(uint16_t i);
void n_build_payload(void);
void n_build_tcp_payload(void);

bool n_create_rr_questionA(string &s);
bool n_create_rr_questionAAAA(string &s);
bool n_create_rr_answer(string &s);
//bool n_create_rr_nameserver(string &s);
//bool n_create_rr_additional(string &s);

// you can use out of class function
size_t n_compress(const char *src, size_t src_size, char *dst, size_t dst_size);

private:

// class member (for class information)
u_char *count_pointer;
//size_t compress_size;
size_t n_size_payload;
u_char n_payload_buf[UDP_PACKET_SIZE];

// class member (for packet header)
uint16_t n_id;    // identifier
uint16_t n_flags; // flags
#define QR         0x8000 // Respons, message is a response.
#define OP0400     0x4000 // Opcode, Standard query (0)
#define OP0200     0x2000 // Opcode, Standard query (0)
#define OP0100     0x1000 // Opcode, Standard query (0)
#define OP0080     0x0800 // Opcode, Standard query (0)
#define AA         0x0400 // Authoritative Answer.
#define TC         0x0200 // Truncated, when your packet data will be over 512.
#define RD         0x0100 // Recursion Desired. 
#define RA         0x0080 // Recursion Available.
#define Z          0x0040 // Zero, reserved (0)
#define AD         0x0020 // authenticated Dataa (for DNSSEC).
#define CD         0x0010 // Checking disabled. (for DNSSEC)
#define RC0008     0x0008 // R code.
#define RC0004     0x0004 // R code.
#define RC0002     0x0002 // R code.
#define RC0001     0x0001 // R code.

// resource recode numver
uint16_t n_qus;   // question number
uint16_t n_ans;   // answer number 
uint16_t n_name;  // nameserver number
uint16_t n_add;   // additional number

// resource recode question

struct _rr_pkt n_rr_qus;
#define n_qus_name  n_rr_qus.rr_name
#define n_qus_size  n_rr_qus.rr_size
#define n_qus_type  n_rr_qus.rr_type
#define n_qus_class n_rr_qus.rr_class

// resource recode answer
struct _rr_pkt n_rr_ans;
#define n_ans_name  n_rr_ans.rr_name
#define n_ans_size  n_rr_ans.rr_size
#define n_ans_type  n_rr_ans.rr_type
#define n_ans_class n_rr_ans.rr_class
#define n_ans_ttl   n_rr_ans.rr_ttl
#define n_ans_rlen  n_rr_ans.rr_rlen
#define n_ans_rdata n_rr_ans.rr_rdata
#define n_ans_raddr n_rr_ans.rr_sin_addr
#define n_ans_raddr6 n_rr_ans.rr_sin6_addr

// resource recode nameserver
struct _rr_pkt n_rr_name;
#define n_name_name  n_rr_name.rr_name
#define n_name_size  n_rr_name.rr_size
#define n_name_type  n_rr_name.rr_type
#define n_name_class n_rr_name.rr_class
#define n_name_ttl   n_rr_name.rr_ttl
#define n_name_rlen  n_rr_name.rr_rlen
#define n_name_rdata n_rr_name.rr_rdata
#define n_name_raddr n_rr_name.rr_sin_addr
#define n_name_raddr6 n_rr_name.rr_sin6_addr

// resource recode addional
struct _rr_pkt n_rr_add;
#define n_add_name  n_rr_add.rr_name
#define n_add_size  n_rr_add.rr_size
#define n_add_type  n_rr_add.rr_type
#define n_add_class n_rr_add.rr_class
#define n_add_ttl   n_rr_add.rr_ttl
#define n_add_rlen  n_rr_add.rr_rlen
#define n_add_rdata n_rr_add.rr_rdata
#define n_add_raddr n_rr_add.rr_sin_addr
#define n_add_raddr6 n_rr_add.rr_sin6_addr

// function
bool n_is_pkt_ok(void);
};

name_pkt::~name_pkt(void) {
};

name_pkt::name_pkt(void) {
    n_init();
};

void name_pkt::n_init(void) {
    // public initialize
    n_id = 0;
    n_flags = 0;
    n_qus = 0;
    n_ans = 0;
    n_name = 0;
    n_add = 0;
    memset(&n_rr_qus, 0, sizeof(n_rr_qus));
    memset(&n_rr_ans, 0, sizeof(n_rr_ans));
    memset(&n_rr_name, 0, sizeof(n_rr_name));
    memset(&n_rr_add, 0, sizeof(n_rr_add));

    // private initialize
    memset(&n_payload_buf, 0, sizeof(n_payload_buf));
    n_size_payload = 0;
    count_pointer = n_payload_buf;

    return;
};


void name_pkt::n_set_id(int i)
{
    n_id = i;  
    return;
};

void name_pkt::n_set_flags(uint16_t i)
{
    n_flags = (n_flags | i); 
    //cout << hex << n_flags << endl;
    return;
};

void name_pkt::n_reset_flags(void)
{
    n_flags = 0; 
    return;
};

bool name_pkt::n_create_rr_questionA(string &s)
{
    // DNS HEADER PART

    // DNS Question Record
    n_qus++;
    //memcpy(npkt.n_rr_qus.rr_name, buf, compress_size);
    //memcpy(npkt.n_qus_name, buf, compress_size);
    //npkt.n_qus_size  = compress_size;
    n_qus_size  = n_compress(s.data(), (size_t)s.size(), n_qus_name, sizeof(n_qus_name));
    n_qus_type  = 1;
    n_qus_class = 1;
    return true;
};

bool name_pkt::n_create_rr_questionAAAA(string &s)
{
    // DNS HEADER PART

    // DNS Question Record
    n_qus++;
    //memcpy(npkt.n_rr_qus.rr_name, buf, compress_size);
    //memcpy(npkt.n_qus_name, buf, compress_size);
    //npkt.n_qus_size  = compress_size;
    n_qus_size  = n_compress(s.data(), (size_t)s.size(), n_qus_name, sizeof(n_qus_name));
    n_qus_type  = 28;
    n_qus_class = 1;
    return true;
};

bool name_pkt::n_create_rr_answer(string &s)
{
    n_ans++;
    //npkt.n_ans_name[0] = 0xC0;
    //npkt.n_ans_name[1] = 0x0C;
    //n_ans_size = 2;

    n_ans_size = n_qus_size;
    memcpy(n_ans_name, n_qus_name, n_qus_size);

    //n_ans_type = 1;
    n_ans_type = n_qus_type;
    n_ans_class= 1;
    n_ans_ttl = 10000;
    if ( n_ans_type == 1) {
        // input IPv4 address
        //cout << "rr_answer ipv4" << endl;
        n_ans_rlen = 4;
        inet_pton(AF_INET, s.data(), &n_ans_raddr);
    } else if ( n_ans_type == 28) {
        // input IPv6 address
        n_ans_rlen = 16;
        inet_pton(AF_INET6, s.data(), &n_ans_raddr6);
    } else {
        // input compress binet name
        n_ans_rlen = s.size();
        memcpy(n_ans_rdata , s.data() , n_ans_rlen);
    }
    return true;
};

/*
bool name_pkt::n_create_rr_nameserver(string &s)
//n_name++;
return false;
};

bool name_pkt::n_create_rr_additional(string &s)
//n_add++;
return false;
};
*/

bool name_pkt::n_is_pkt_ok(void) {
    return false;
};

void name_pkt::n_build_tcp_payload(void)
{
    uint16_t n_id_nw_s    = htons(n_id);
    uint16_t n_flags_nw_s = htons(n_flags);
    uint16_t n_qus_nw_s   = htons(n_qus);
    uint16_t n_ans_nw_s   = htons(n_ans);
    uint16_t n_name_nw_s  = htons(n_name);
    uint16_t n_add_nw_s   = htons(n_add);


    count_pointer++;
    count_pointer++;
    N_WRITE16(n_id_nw_s, count_pointer);
    N_WRITE16(n_flags_nw_s, count_pointer);
    N_WRITE16(n_qus_nw_s, count_pointer);
    N_WRITE16(n_ans_nw_s, count_pointer);
    N_WRITE16(n_name_nw_s, count_pointer);
    N_WRITE16(n_add_nw_s, count_pointer);

    if (n_qus > 0) {
        memcpy(count_pointer, n_qus_name, (int)n_qus_size);
        for (size_t i=0; i<n_qus_size; i++) count_pointer++;

        uint16_t n_qus_type_nw_s = htons(n_qus_type);
        N_WRITE16(n_qus_type_nw_s, count_pointer);

        uint16_t n_qus_class_nw_s = htons(n_qus_class);
        N_WRITE16(n_qus_class_nw_s, count_pointer);
    }

    if (n_ans > 0) {
        memcpy(count_pointer, n_ans_name, (int)n_ans_size);
        for (size_t i=0; i<n_qus_size; i++) count_pointer++;

        uint16_t n_ans_type_nw_s = htons(n_ans_type);
        N_WRITE16(n_ans_type_nw_s, count_pointer);

        uint16_t n_ans_class_nw_s = htons(n_ans_class);
        N_WRITE16(n_ans_class_nw_s, count_pointer);

        uint32_t n_ans_ttl_nw_s = htonl(n_ans_ttl);
        N_WRITE32(n_ans_ttl_nw_s, count_pointer);

        uint16_t n_ans_rlen_nw_s = htons(n_ans_rlen);
        N_WRITE16(n_ans_rlen_nw_s, count_pointer);

        if (n_ans_type == 1) {
            N_WRITE32(n_ans_raddr.s_addr, count_pointer);
        } else if (n_ans_type == 28) { 
            // IPv6 name type response
#ifdef __linux__
            N_WRITE128(n_ans_raddr6.__in6_u, count_pointer);
#else
            N_WRITE128(n_ans_raddr6.__u6_addr, count_pointer);
#endif
        } else {
            memcpy(count_pointer, n_ans_rdata, (int)n_ans_rlen);
            for (size_t i=0; i<n_qus_size; i++) count_pointer++;
        }
    }

    if (n_name > 0) {
        memcpy(count_pointer, n_name_name, (int)n_name_size);
        for (size_t i=0; i<n_qus_size; i++) count_pointer++;

        uint16_t n_name_type_nw_s = htons(n_name_type);
        N_WRITE16(n_name_type_nw_s, count_pointer);

        uint16_t n_name_class_nw_s = htons(n_name_class);
        N_WRITE16(n_name_class_nw_s, count_pointer);

        uint32_t n_name_ttl_nw_s = htonl(n_name_ttl);
        N_WRITE32(n_name_ttl_nw_s, count_pointer);

        uint16_t n_name_rlen_nw_s = htons(n_name_rlen);
        N_WRITE16(n_name_rlen_nw_s, count_pointer);

        if (n_name_type == 1) {
            N_WRITE32(n_name_raddr.s_addr, count_pointer);
        } else if (n_name_type == 28) { 
            // IPv6 name type response
#ifdef __linux__
            N_WRITE128(n_ans_raddr6.__in6_u, count_pointer);
#else
            N_WRITE128(n_ans_raddr6.__u6_addr, count_pointer);
#endif
        } else {
            memcpy(count_pointer, n_name_rdata, (int)n_name_rlen);
            for (size_t i=0; i<n_qus_size; i++) count_pointer++;
        }
    }

    if (n_add > 0) {
        memcpy(count_pointer, n_add_name, (int)n_add_size);
        for (size_t i=0; i<n_qus_size; i++) count_pointer++;

        uint16_t n_add_type_nw_s = htons(n_add_type);
        N_WRITE16(n_add_type_nw_s, count_pointer);

        uint16_t n_add_class_nw_s = htons(n_add_class);
        N_WRITE16(n_add_class_nw_s, count_pointer);

        uint32_t n_add_ttl_nw_s = htonl(n_add_ttl);
        N_WRITE32(n_add_ttl_nw_s, count_pointer);

        uint16_t n_add_rlen_nw_s = htons(n_add_rlen);
        N_WRITE16(n_add_rlen_nw_s, count_pointer);

        if (n_add_type == 1) {
            N_WRITE32(n_add_raddr.s_addr, count_pointer);
        } else if (n_add_type == 28) {
            // IPv6 name type response
#ifdef __linux__
            N_WRITE128(n_ans_raddr6.__in6_u, count_pointer);
#else
            N_WRITE128(n_ans_raddr6.__u6_addr, count_pointer);
#endif
        } else {
            memcpy(count_pointer, n_add_rdata, (int)n_add_rlen);
            for (size_t i=0; i<n_qus_size; i++) count_pointer++;
        }
    }


    n_size_payload = (size_t)(count_pointer - n_payload_buf);
    //printf("base  : %p\n", n_payload_buf);
    //printf("shift : %p\n", count_pointer);
    //printf("byte  : %d\n", n_size_payload);

    uint16_t* msg_size = (uint16_t*)n_payload_buf;
    *msg_size = htons((uint16_t)(n_size_payload-2));

    return;
};

void name_pkt::n_build_payload(void)
{
    uint16_t n_id_nw_s    = htons(n_id);
    uint16_t n_flags_nw_s = htons(n_flags);
    uint16_t n_qus_nw_s   = htons(n_qus);
    uint16_t n_ans_nw_s   = htons(n_ans);
    uint16_t n_name_nw_s  = htons(n_name);
    uint16_t n_add_nw_s   = htons(n_add);

    N_WRITE16(n_id_nw_s, count_pointer);
    N_WRITE16(n_flags_nw_s, count_pointer);
    N_WRITE16(n_qus_nw_s, count_pointer);
    N_WRITE16(n_ans_nw_s, count_pointer);
    N_WRITE16(n_name_nw_s, count_pointer);
    N_WRITE16(n_add_nw_s, count_pointer);

    if (n_qus > 0) {
        memcpy(count_pointer, n_qus_name, (int)n_qus_size);
        for (size_t i=0; i<n_qus_size; i++) count_pointer++;

        uint16_t n_qus_type_nw_s = htons(n_qus_type);
        N_WRITE16(n_qus_type_nw_s, count_pointer);

        uint16_t n_qus_class_nw_s = htons(n_qus_class);
        N_WRITE16(n_qus_class_nw_s, count_pointer);
    }

    if (n_ans > 0) {
        memcpy(count_pointer, n_ans_name, (int)n_ans_size);
        for (size_t i=0; i<n_qus_size; i++) count_pointer++;

        uint16_t n_ans_type_nw_s = htons(n_ans_type);
        N_WRITE16(n_ans_type_nw_s, count_pointer);

        uint16_t n_ans_class_nw_s = htons(n_ans_class);
        N_WRITE16(n_ans_class_nw_s, count_pointer);

        uint32_t n_ans_ttl_nw_s = htonl(n_ans_ttl);
        N_WRITE32(n_ans_ttl_nw_s, count_pointer);

        uint16_t n_ans_rlen_nw_s = htons(n_ans_rlen);
        N_WRITE16(n_ans_rlen_nw_s, count_pointer);

        if (n_ans_type == 1) {
            N_WRITE32(n_ans_raddr.s_addr, count_pointer);
        } else if (n_ans_type == 28) { 
            // IPv6 name type response
#ifdef __linux__
            N_WRITE128(n_ans_raddr6.__in6_u, count_pointer);
#else
            N_WRITE128(n_ans_raddr6.__u6_addr, count_pointer);
#endif
        } else {
            memcpy(count_pointer, n_ans_rdata, (int)n_ans_rlen);
            for (size_t i=0; i<n_qus_size; i++) count_pointer++;
        }
    }

    if (n_name > 0) {
        memcpy(count_pointer, n_name_name, (int)n_name_size);
        for (size_t i=0; i<n_qus_size; i++) count_pointer++;

        uint16_t n_name_type_nw_s = htons(n_name_type);
        N_WRITE16(n_name_type_nw_s, count_pointer);

        uint16_t n_name_class_nw_s = htons(n_name_class);
        N_WRITE16(n_name_class_nw_s, count_pointer);

        uint32_t n_name_ttl_nw_s = htonl(n_name_ttl);
        N_WRITE32(n_name_ttl_nw_s, count_pointer);

        uint16_t n_name_rlen_nw_s = htons(n_name_rlen);
        N_WRITE16(n_name_rlen_nw_s, count_pointer);

        if (n_name_type == 1) {
            N_WRITE32(n_name_raddr.s_addr, count_pointer);
        } else if (n_name_type == 28) { 
            // IPv6 name type response
#ifdef __linux__
            N_WRITE128(n_ans_raddr6.__in6_u, count_pointer);
#else
            N_WRITE128(n_ans_raddr6.__u6_addr, count_pointer);
#endif
        } else {
            memcpy(count_pointer, n_name_rdata, (int)n_name_rlen);
            for (size_t i=0; i<n_qus_size; i++) count_pointer++;
        }
    }

    if (n_add > 0) {
        memcpy(count_pointer, n_add_name, (int)n_add_size);
        for (size_t i=0; i<n_qus_size; i++) count_pointer++;

        uint16_t n_add_type_nw_s = htons(n_add_type);
        N_WRITE16(n_add_type_nw_s, count_pointer);

        uint16_t n_add_class_nw_s = htons(n_add_class);
        N_WRITE16(n_add_class_nw_s, count_pointer);

        uint32_t n_add_ttl_nw_s = htonl(n_add_ttl);
        N_WRITE32(n_add_ttl_nw_s, count_pointer);

        uint16_t n_add_rlen_nw_s = htons(n_add_rlen);
        N_WRITE16(n_add_rlen_nw_s, count_pointer);

        if (n_add_type == 1) {
            N_WRITE32(n_add_raddr.s_addr, count_pointer);
        } else if (n_add_type == 28) {
            // IPv6 name type response
#ifdef __linux__
            N_WRITE128(n_ans_raddr6.__in6_u, count_pointer);
#else
            N_WRITE128(n_ans_raddr6.__u6_addr, count_pointer);
#endif
        } else {
            memcpy(count_pointer, n_add_rdata, (int)n_add_rlen);
            for (size_t i=0; i<n_qus_size; i++) count_pointer++;
        }
    }

    n_size_payload = (size_t)(count_pointer - n_payload_buf);
    //printf("base  : %p\n", n_payload_buf);
    //printf("shift : %p\n", count_pointer);
    //printf("byte  : %d\n", n_size_payload);

    return;
};

u_char *name_pkt::n_payload(void) {
    return n_payload_buf;
}

size_t name_pkt::n_payload_size(void) {
    return n_size_payload;
};

size_t name_pkt::n_compress(const char *src, size_t src_size, char *dst, size_t dst_size)
{

    if (src_size == 0) return 0;

    string::size_type i;
    string::size_type j;
    vector<string> v;
    string s(src,src_size);

    while (s[s.size()-1] == '\0') {
        s = s.substr(0,s.size()-1);
    }

    v.clear();

    i = 0;
    j = s.find(".");

    while(j != string::npos){
        if (i == 0) v.push_back(s.substr(i,j-i));
        if (i != 0) v.push_back(s.substr(i+1,j-i-1));
        i = j++;
        j = s.find(".", j);
        if (j == string::npos){
            v.push_back(s.substr(i+1, s.size()));
            break;
        }
    }

    string compress_s;
    for (unsigned int i=0; i< v.size(); i++) {
        compress_s += (char)v[i].size();
        compress_s += v[i];
    }
    compress_s += (char)0;

    /*// check compress name
      for (unsigned int i=0; i< compress_s.size(); i++) {
      char a = compress_s.at(i);
      printf("%3x",a);
      printf("  ",a);
      printf("%1c ",a);
      printf("\n");
      }
      */
    if (dst_size < compress_s.size()) return 0;
    memcpy(dst, compress_s.data(), compress_s.size());
    return (size_t)compress_s.size();
}
#endif
