#include "../src/dgram.hpp"

#include <string.h>

void
usage()
{
    std::cout << "Usage: dgram_server [port]" << std::endl;
    return;
}

int main(int argc, char** argv)
{

    debug =true;

    ssize_t size;
    char buf[BUFSIZ];

    class dgram udp;

    if (argc != 2) {
        usage();
        exit(EXIT_FAILURE);
    }

    udp.dgram_open("AF_INET");
    udp.dgram_bind("0.0.0.0", argv[1]);

    for (;;) {
        memset(buf, 0, sizeof(buf));
        size = udp.dgram_recvfrom(buf, sizeof(buf));
        if (size < 0) { continue; }

        std::cout << udp.dgram_get_from_host();
        std::cout << ":";
        std::cout << udp.dgram_get_from_port();
        std::cout << " ";
        std::cout << "(" << size << ")";
        std::cout << buf << std::endl;

        udp.dgram_set_from2to();
        udp.dgram_sendto(buf, size, 0);
    }
    return 0;
}
