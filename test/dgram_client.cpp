#include "../src/dgram.hpp"

#include <stdlib.h>

void
usage()
{
    std::cout << "Usage: dgram_client [server] [port] [message]" << std::endl;
    return;
}

int main(int argc, char** argv)
{
    char buf[BUFSIZ];

    class dgram udp;

    if (argc != 4) {
        usage();
        exit(EXIT_FAILURE);
    }

    if (udp.dgram_open("AF_INET") < 0) exit(EXIT_FAILURE);
    if (udp.dgram_set_to(argv[1], argv[2]) == false) exit(EXIT_FAILURE);

    for (;;) {
        std::cout << "send: " << udp.dgram_sendto(argv[3], strlen(argv[3]), 0) << std::endl;
        std::cout << "recv: " << udp.dgram_recvfrom(buf, sizeof(buf)) << std::endl;
        sleep(1);
    }
    return 0;
}
