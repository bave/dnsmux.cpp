#ifndef __NS_PARSER_HPP__
#define __NS_PARSER_HPP__


#ifdef __APPLE__
#include <nameser.h>
#else
#include <arpa/nameser.h>
#endif

#include "common.hpp"

// XXX
class ns_parser
{
public:
    ns_parser() { };
    virtual ~ns_parser() { };

    bool set_msg(char* msg, size_t length); 
    int get_id();
    void print_query();
    void print_answer();
    void nsdump();

    char* buf[BUFSIZ];
    ns_msg ns_handle;

private:
};

bool
ns_parser::set_msg(char* msg, size_t length)
{
    memset(&ns_handle, 0, sizeof(ns_handle));
    if(ns_initparse((const unsigned char*)msg, length, &ns_handle)) {
        return false;
    } else {
        return true;
    }
}

int
ns_parser::get_id()
{
    return ns_msg_id(ns_handle);
}

void
ns_parser::print_query()
{
    int i;
    ns_rr rr;
    char pbuf[0xFFFF];
    memset(pbuf, 0, sizeof(pbuf));

    int name_count_qd;
    name_count_qd = ns_msg_count(ns_handle, ns_s_qd);
    for (i = 0; i < name_count_qd; i++) {
        memset(&rr, 0, sizeof(rr));
        if (ns_parserr(&ns_handle, ns_s_qd, i, &rr) == 0) {
            ns_sprintrr(&ns_handle, &rr, NULL, NULL, pbuf, sizeof(pbuf));
            printf("query : %s\n", pbuf);
            //printf("\tfqdn :%s\n", ns_rr_name(rr));
        }
    }
    return;
}

void
ns_parser::print_answer()
{
    int i;
    ns_rr rr;
    char pbuf[0xFFFF];
    memset(pbuf, 0, sizeof(pbuf));

    int name_count_an;
    name_count_an = ns_msg_count(ns_handle, ns_s_an);
    for (i = 0; i < name_count_an; i++) {
        memset(&rr, 0, sizeof(rr));
        if (ns_parserr(&ns_handle, ns_s_an, i, &rr) == 0) {
            ns_sprintrr(&ns_handle, &rr, NULL, NULL, pbuf, sizeof(pbuf));
            printf("answer: %s\n", pbuf);
            //printf("\tfqdn :%s\n", ns_rr_name(rr));
        }
    }
    return;
}

void
ns_parser::nsdump()
{
    int i;

    /*
     * -- identifier
     */
    int name_id = ns_msg_id(ns_handle);
    printf("-- identifier -\n");
    printf("%d\n", name_id);
    printf("---------------\n");

    /*
     * -- flags --
     * 0 1 5 6 7 8 11 15
     * +----+----------------+----+----+----+-----------+----------------+
     * | QR | Operation Code | AA | TC | RA |    Zero   |     Recode     |
     * +----+----------------+----+----+----+-----------+----------------+
     *
     * Question/Response          : ns_f_qr
     * Operation code             : ns_f_opcode
     * Authoritative Answer       : ns_f_aa
     * Truncation occurred        : ns_f_tc
     * Recursion Desired          : ns_f_rd
     * Recursion Available        : ns_f_ra
     * MBZ                        : ns_f_z
     * Authentic Data (DNSSEC)    : ns_f_ad
     * Checking Disabled (DNSSEC) : ns_f_cd
     * Response code              : ns_f_rcode
     */
    printf("-- flag -------\n");
    printf("ns_f_qr : %d\n", ns_msg_getflag(ns_handle, ns_f_qr));
    printf("ns_f_opcode: %d\n", ns_msg_getflag(ns_handle, ns_f_opcode));
    printf("ns_f_aa : %d\n", ns_msg_getflag(ns_handle, ns_f_aa));
    printf("ns_f_tc : %d\n", ns_msg_getflag(ns_handle, ns_f_tc));
    printf("ns_f_rd : %d\n", ns_msg_getflag(ns_handle, ns_f_rd));
    printf("ns_f_ra : %d\n", ns_msg_getflag(ns_handle, ns_f_ra));
    printf("ns_f_z : %d\n", ns_msg_getflag(ns_handle, ns_f_z));
    printf("ns_f_ad : %d\n", ns_msg_getflag(ns_handle, ns_f_ad));
    printf("ns_f_cd : %d\n", ns_msg_getflag(ns_handle, ns_f_cd));
    printf("ns_f_rcode : %d\n", ns_msg_getflag(ns_handle, ns_f_rcode));
    printf("---------------\n");

    /*
       Question ns_s_qd
       Answer ns_s_an
       Name servers ns_s_ns
       Add records ns_s_ar
       */
    int name_count_qd = ns_msg_count(ns_handle, ns_s_qd);
    int name_count_an = ns_msg_count(ns_handle, ns_s_an);
    int name_count_ns = ns_msg_count(ns_handle, ns_s_ns);
    int name_count_ar = ns_msg_count(ns_handle, ns_s_ar);
    /*
       printf("-- count ------\n");
       printf("ns_s_qd : %d\n", name_count_qd);
       printf("ns_s_an : %d\n", name_count_an);
       printf("ns_s_ns : %d\n", name_count_ns);
       printf("ns_s_ar : %d\n", name_count_ar);
       printf("---------------\n");
       */

    ns_rr rr;
    char pbuf[0xFFFF];

    // query
    printf("Questionrecord\n");
    for (i = 0; i < name_count_qd; i++) {
        memset(&rr, 0, sizeof(rr));
        if (ns_parserr(&ns_handle, ns_s_qd, i, &rr) == 0) {
            printf("\t-- %d --\n", i+1);
            ns_sprintrr(&ns_handle, &rr, NULL, NULL, pbuf, sizeof(pbuf));
            printf("\t%s\n", pbuf);
            printf("\tfqdn :%s\n", ns_rr_name(rr));
            printf("\ttype :%d\n", ns_rr_type(rr));
            printf("\tclass:%d\n", ns_rr_class(rr));
            printf("\tttl :%d\n", ns_rr_ttl(rr));
            //printf("\trdlen:%d\n", ns_rr_rdlen(rr));
            //hexdump("qd", (uint8_t*)ns_rr_rdata(rr), ns_rr_rdlen(rr));
        }
    }

    printf("AnswerRecord\n");
    for (i = 0; i < name_count_an; i++) {
        memset(&rr, 0, sizeof(rr));
        if (ns_parserr(&ns_handle, ns_s_an, i, &rr) == 0) {
            printf("\t-- %d --\n", i+1);
            ns_sprintrr(&ns_handle, &rr, NULL, NULL, pbuf, sizeof(pbuf));
            printf("\t%s\n", pbuf);
            printf("\tfqdn :%s\n", ns_rr_name(rr));
            printf("\ttype :%d\n", ns_rr_type(rr));
            printf("\tclass:%d\n", ns_rr_class(rr));
            printf("\tttl :%d\n", ns_rr_ttl(rr));
            //printf("\trdlen:%d\n", ns_rr_rdlen(rr));
            //hexdump("an", (uint8_t*)ns_rr_rdata(rr), ns_rr_rdlen(rr));
        }
    }

    printf("NameServerRecord\n");
    for (i = 0; i < name_count_ns; i++) {
        memset(&rr, 0, sizeof(rr));
        if (ns_parserr(&ns_handle, ns_s_ns, i, &rr) == 0) {
            printf("\t-- %d --\n", i+1);
            ns_sprintrr(&ns_handle, &rr, NULL, NULL, pbuf, sizeof(pbuf));
            printf("\t%s\n", pbuf);
            printf("\tfqdn :%s\n", ns_rr_name(rr));
            printf("\ttype :%d\n", ns_rr_type(rr));
            printf("\tclass:%d\n", ns_rr_class(rr));
            printf("\tttl :%d\n", ns_rr_ttl(rr));
            //printf("\trdata:%s\n", ns_rr_rdata(rr));
            //hexdump("ns", (uint8_t*)ns_rr_rdata(rr), ns_rr_rdlen(rr));
        }
    }

    printf("AdditionalRecord\n");
    for (i = 0; i < name_count_ar; i++) {
        memset(&rr, 0, sizeof(rr));
        if (ns_parserr(&ns_handle, ns_s_ar, i, &rr) == 0) {
            printf("\t-- %d --\n", i+1);
            ns_sprintrr (&ns_handle, &rr, NULL, NULL, pbuf, sizeof(pbuf));
            printf("\t%s\n", pbuf);
            printf("\tfqdn :%s\n", ns_rr_name(rr));
            printf("\ttype :%d\n", ns_rr_type(rr));
            printf("\tclass:%d\n", ns_rr_class(rr));
            printf("\tttl :%d\n", ns_rr_ttl(rr));
            //printf("\trdlen:%d\n", ns_rr_rdlen(rr));
            //hexdump("ar", (uint8_t*)ns_rr_rdata(rr), ns_rr_rdlen(rr));
        }
    }
    return;
}

#endif
