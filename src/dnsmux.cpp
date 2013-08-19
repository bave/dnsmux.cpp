#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <event2/event.h>
#include <getopt.h>

#include <iostream>

#include "common.hpp"
#include "utils.hpp"
#include "id_table.hpp"
#include "event.hpp"

void
version()
{
    printf("version 0.0.1");
    return;
}

void
usage()
{
    printf( "Usage: dnsmux [options]\n"
            " * show help command\n"
            "    -h, --help\n"
            " * show version of dnsmux\n"
            "    -v, --version\n"
            " * show debug messages\n"
            "    -d, --debug\n"
            " * one query use one tcp sesstion\n"
            "    -1, --one\n"
            " * proxy service port (default: 53)\n"
            "    -l, --local <value>\n"
            " * dns server port (default: 53)\n"
            "    -p, --port <value>\n"
            " * dns server ip (default: 8.8.8.8)\n"
            "    -s, --server <value>\n");
    return;
}

int
main(int argc, char** argv)
{

    id_table t;
    event_base* ev_base = event_base_new();

    char* opt_l = "localhost";
    char* opt_p = "53";
    char* opt_s = "8.8.8.8";


    int opt;
    int option_index;
    struct option long_options[] = {
        {"help" ,   no_argument,       NULL, 'h'},
        {"one",     no_argument,       NULL, '1'},
        {"version", no_argument,       NULL, 'v'},
        {"dns",     required_argument, NULL, 's'},
        {"addr",    required_argument, NULL, 'l'},
        {"port",    required_argument, NULL, 'p'},
#ifdef DEBUG
        {"debug",   no_argument,       NULL, 'd'},
#endif
        {0, 0, 0, 0}
    };

    //opt, optarg;
    while((opt = getopt_long(argc, argv, "hd1vs:l:p:", long_options, &option_index)) != -1){
        switch(opt){
#ifdef DEBUG
            case 'd':
                debug = true;
                break;
#endif
            case 'h':
                usage();
                exit(EXIT_FAILURE);
                break;
            case '1':
                one = true;
                break;
            case 'v':
                version();
                break;
            case 's':
                opt_s = optarg;
                break;
            case 'l':
                printf("opt: %c, optarg: %s\n", opt, optarg);
                opt_l = optarg;
                break;
            case 'p':
                opt_p = optarg;
                break;
            case '?':
                exit(EXIT_FAILURE);
                break;
            default:
                exit(EXIT_FAILURE);
        }
    }


    ev_dgram udp4;
    udp4.dgram_open("AF_INET");
    udp4.dgram_bind(opt_l, opt_p);

    ev_dgram udp6;
    udp6.dgram_open("AF_INET6");
    udp6.dgram_bind(opt_l, opt_p);

    ev_stream tcp;
    tcp.stream_open("AF_INET");
    if (tcp.stream_connect(opt_s, "53") == false) exit(EXIT_FAILURE);

    udp4.ed_set_ev_base(ev_base);
    udp4.set_opposite_stream(&tcp);
    udp4.set_id_table(&t);
    udp4.ed_dispatch();

    udp6.ed_set_ev_base(ev_base);
    udp6.set_opposite_stream(&tcp);
    udp6.set_id_table(&t);
    udp6.ed_dispatch();

    tcp.es_set_ev_base(ev_base);
    tcp.set_opposite_dgram(static_cast<dgram*>(&udp4), static_cast<dgram*>(&udp6));
    tcp.set_id_table(&t);
    tcp.es_dispatch();

    event_base_dispatch(ev_base); 

    exit(EXIT_SUCCESS);
}
