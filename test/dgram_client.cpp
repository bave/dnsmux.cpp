#include "../src/dgram.hpp"

int main()
{
    char buf[BUFSIZ];

    class dgram udp;
    udp.dgram_open("AF_INET");
    for (;;) {
        udp.dgram_set_to("aris.jaist.ac.jp", "10443");
        std::cout << "send: " << udp.dgram_sendto("hogehogehoge", strlen("hogehogehoge"), 0) << std::endl;
        std::cout << "recv: " << udp.dgram_recvfrom(buf, sizeof(buf)) << std::endl;
        sleep(1);
    }
    return 0;
}
