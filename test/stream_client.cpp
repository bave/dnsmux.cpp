#include "../src/stream.hpp"

#include <stdlib.h>

void
usage()
{
    std::cout << "Usage: stream_client [server] [port] [message]" << std::endl;
    return;
}

int main(int argc, char** argv)
{

    debug = true;

    char buf[BUFSIZ];

    class stream tcp;

    if (argc != 4) {
        usage();
        exit(EXIT_FAILURE);
    }

    tcp.stream_open("AF_INET");
    tcp.stream_connect(argv[1], argv[2]);
    for (;;) {
        std::cout << "send: " << tcp.stream_send(argv[3], strlen(argv[3]), 0) << std::endl;
        sleep(1);
        std::cout << "recv: " << tcp.stream_recv(buf, sizeof(buf))  << std::endl;
        tcp.stream_close();
        tcp.stream_reconnect();
    }
    return 0;
}
