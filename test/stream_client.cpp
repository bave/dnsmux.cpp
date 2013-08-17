#include "../src/stream.hpp"

int main()
{
    char buf[BUFSIZ];

    class stream tcp;

    tcp.stream_open("AF_INET");
    tcp.stream_connect("aris.jaist.ac.jp", "10443");
    for (;;) {
        std::cout << "send: " << tcp.stream_send("hogehoge", sizeof("hogehoge"), 0) << std::endl;
        std::cout << "recv: " << tcp.stream_recv(buf, sizeof(buf))  << std::endl;
        tcp.stream_close();
        sleep(1);
        tcp.stream_reconnect();
    }
    return 0;
}
