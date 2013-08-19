#include "../src/stream.hpp"

#include <string.h>

void
usage()
{
    std::cout << "Usage: stream_server [port]" << std::endl;
    return;
}

int
main(int argc, char** argv)
{
    debug = true;

    ssize_t size;
    char buf[BUFSIZ];

    class stream tcp;

    if (argc != 2) {
        usage();
        exit(EXIT_FAILURE);
    }

    tcp.stream_open("AF_INET");
    tcp.stream_listen("0.0.0.0", argv[1]);

    for (;;) {

        tcp.stream_accept();

        for (;;) {

            memset(buf, 0, sizeof(buf));
            size = tcp.stream_recv(buf, sizeof(buf));
            if (size <= 0) { break; } 

            std::cout << tcp.stream_get_peer_host();
            std::cout << ":";
            std::cout << tcp.stream_get_peer_port();
            std::cout << " ";
            std::cout << "(" << size << ")";
            std::cout << buf << std::endl;

            tcp.stream_send(buf, size, 0);

        }

    }

    return 0;
}
