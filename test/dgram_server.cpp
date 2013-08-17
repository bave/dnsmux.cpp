#include "../src/dgram.hpp"

int main()
{
    char buf[BUFSIZ];

    class dgram udp;
    udp.dgram_open("AF_INET");
    for (;;) {
        std::cout << "recv: " << udp.dgram_recvfrom(buf, sizeof(buf)) << std::endl;
        udp.dgram_set_from2to();
        std::cout << "send: " << udp.dgram_sendto(buf, strlen(buf), 0) << std::endl;
    }
    return 0;
}
