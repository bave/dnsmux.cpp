#include "../src/stream.hpp"
#include <string.h>

int main()
{
    char buf[BUFSIZ];
    class stream tcp;
    tcp.stream_open("AF_INET");
    tcp.stream_listen("0.0.0.0", "10443");
    ssize_t size;
    for (;;) {
        tcp.stream_accept();
        for (;;) {
            memset(buf, 0, BUFSIZ);
            size = tcp.stream_recv(buf, sizeof(buf));
            printf("%s\n", buf);
            if (size <= 0) {
                break;
            } 
            sleep(1);
            tcp.stream_send(buf, strlen(buf), 0);
        }
    }
    return 0;
}
