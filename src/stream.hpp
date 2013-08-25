#ifndef __STREAM_HPP__
#define __STREAM_HPP__


/*
// for Information of TFO flag

sysctl net.ipv4.tcp_fastopen
or
file:////proc/sys/net/ipv4/tcp_fastopen

// Bit Flags for sysctl_tcp_fastopen
#define TFO_CLIENT_ENABLE       1
#define TFO_SERVER_ENABLE       2

// Data in SYN w/o cookie option
#define TFO_CLIENT_NO_COOKIE    4

// Process SYN data but skip cookie validation
#define TFO_SERVER_COOKIE_NOT_CHKED     0x100

//Accept SYN data w/o any cookie option
#define TFO_SERVER_COOKIE_NOT_REQD      0x200

// Force enable TFO on all listeners, i.e., not requiring the
// TCP_FASTOPEN socket option. SOCKOPT1/2 determine how to set
// max_qlen. Always create TFO child sockets on a TFO listener
// even. when cookie/data not present. (For testing purpose!)
#define TFO_SERVER_WO_SOCKOPT1  0x400
#define TFO_SERVER_WO_SOCKOPT2  0x800

// All listen socket to use tfo
#define TFO_SERVER_ALWAYS       0x1000
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <err.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include <iostream>
#include <map>

#ifdef __linux__
#include <netinet/tcp.h>
#include <x86_64-linux-gnu/bits/socket.h>
#endif

#include "common.hpp"

#ifdef DEBUG
#define STREAM_PERROR(func) do {                                \
    if (debug) {                                                \
    char s[BUFSIZ];                                             \
    memset(s, 0, BUFSIZ);                                       \
    snprintf(s, BUFSIZ, "%s(%d):%s", __FILE__, __LINE__, func); \
    perror(s);                                                  \
    }                                                           \
} while (false)
#else
#define STREAM_PERROR(func) do {} while (false)
#endif

#define STREAM_UNUSED  0
#define STREAM_CONNECT 1
#define STREAM_LISTEN  2

class stream {

public:
    stream() {
        signal(SIGPIPE, SIG_IGN); 
        stream_family    = 0;
        stream_fd = -1;
        stream_accept_fd = -1;
        stream_status = STREAM_UNUSED;
        memset(&stream_peer_sockaddr, 0, sizeof(stream_peer_sockaddr));
    };
    virtual ~stream() {
        if (stream_fd != -1) {
            stream_close();
        }
        if (stream_accept_fd != -1) {
            stream_accept_close();
        }
        
    };

    int stream_open(const char* type);
    int stream_open(const std::string& type);

    ssize_t stream_send(const void* buf, size_t length, int flag);
    ssize_t stream_recv(void* buf, size_t length);

#ifdef __linux__
    ssize_t stream_sendto(const void* buf, size_t length,
                          const std::string& dest_host,
                          const std::string& dest_port);
    ssize_t stream_re_sendto(const void* buf, size_t length);
#endif

    bool stream_close();
    bool stream_accept_close();
    bool stream_reconnect();

    bool stream_connect(const std::string& dest_host, const std::string& dest_port);
    bool stream_connect(const char* dest_host, const char* dest_port);
    bool stream_connect(const struct sockaddr* dest_addr, socklen_t dest_length);

    bool stream_listen(const std::string& host, const std::string& port);
    int stream_accept();

    struct sockaddr* stream_get_peer_sockaddr();
    std::string stream_get_peer_host();
    std::string stream_get_peer_port();

    int get_fd();


private:
    int stream_fd;
    int stream_accept_fd;
    int stream_family;
    int stream_status;

    char stream_buf[BUFSIZ];

    struct sockaddr_storage stream_peer_sockaddr;
    socklen_t stream_peer_addr_length;
    struct sockaddr_storage stream_accept_sockaddr;
    socklen_t stream_accept_addr_length;
};


#ifdef __linux__
ssize_t
stream::stream_sendto(const void* buf, size_t length,
                      const std::string& dest_host,
                      const std::string& dest_port)
{
    int error;
    struct addrinfo *res;
    struct addrinfo hints;

    if (stream_family == 0) {
        return false;
    }

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = stream_family;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_ALL | AI_ADDRCONFIG;

    error = getaddrinfo(dest_host.c_str(), dest_port.c_str(), &hints, &res);
    if (error) {
#ifdef DEBUG
        if (debug) {
        fprintf(stderr, "%s(%d):getaddrinfo: %s\n", __FILE__, __LINE__, gai_strerror(error));
        }
#endif
        return -1;
    }

    ssize_t size;
    size = sendto(stream_fd, buf, length, MSG_FASTOPEN, res->ai_addr, res->ai_addrlen);
    if (size < 0) {

        STREAM_PERROR("sendto");
        freeaddrinfo(res);
        stream_close();
        return -1;

    } else if (size == 0) {

        freeaddrinfo(res);
        stream_close();
        return size;

    } else {

        stream_status = STREAM_CONNECT;
        stream_get_peer_sockaddr();
        freeaddrinfo(res);
        return size;

    }

    return size;
}

ssize_t
stream::stream_re_sendto(const void* buf, size_t length)
{

    if (stream_family == AF_INET) {
        stream_open("AF_INET");
    } else if (stream_family == AF_INET6){
        stream_open("AF_INET6");
    } else if (stream_family == 0) {
        return -1;
    } else {
        return -1;
    }

    if (stream_status == STREAM_CONNECT) {
        ssize_t size;
        size = sendto(stream_fd, buf, length, MSG_FASTOPEN,
                      (struct sockaddr*)&stream_peer_sockaddr,
                      stream_peer_addr_length);

        if (size <= 0) {

            STREAM_PERROR("sendto");
            stream_close();
            return -1;

        } else {

            stream_status = STREAM_CONNECT;
            stream_get_peer_sockaddr();
            return size;

        }

    } else {

        stream_close();
        return -1;

    }

    return -1;
}
#endif

ssize_t
stream::stream_send(const void* buf, size_t length, int flag)
{
    ssize_t ret;

    if (stream_status == STREAM_CONNECT) {

        ret = send(stream_fd, buf, length, flag);
        if (ret < 0) {
            STREAM_PERROR("sendto");
            stream_close();
        } else if (ret == 0) {
            stream_close();
        }
        return ret;

    } else if (stream_status == STREAM_LISTEN) {

        ret = send(stream_accept_fd, buf, length, flag);
        if (ret < 0) {
            STREAM_PERROR("sendto");
            stream_accept_close();
        } else if (ret == 0) {
            stream_accept_close();
        }
        return ret;

    } else {

        return -1;

    }
}

ssize_t
stream::stream_recv(void* buf, size_t length)
{
    if (stream_status == STREAM_CONNECT) {

        ssize_t ret = recv(stream_fd, buf, length, 0);
        if (ret < 0) {
            STREAM_PERROR("recv");
            stream_close();
        } else if (ret == 0) {
            stream_close();
        }
        return ret;

    } else if (stream_status == STREAM_LISTEN) {

        ssize_t ret = recv(stream_accept_fd, buf, length, 0);
        if (ret < 0) {
            STREAM_PERROR("recv");
            stream_accept_close();
        } else if (ret == 0) {
            stream_accept_close();
        }
        return ret;

    } else {

        return -1;

    }

}

bool
stream::stream_close()
{
    int ret;
    ret = close(stream_fd);
    if (ret < 0) {
        return false;
    } else {
        stream_fd = -1;
        return true;
    }
}

bool
stream::stream_accept_close()
{
    int ret;
    ret = close(stream_accept_fd);
    if (ret < 0) {
        return false;
    } else {
        stream_accept_fd = -1;
        return true;
    }
}

bool
stream::stream_reconnect()
{
STREAM_RECONNECT_LOOP:
    if (stream_family == AF_INET) {
        stream_open("AF_INET");
    } else if (stream_family == AF_INET6){
        stream_open("AF_INET6");
    } else if (stream_family == 0) {
        return false;
    } else {
        return false;
    }

    if (stream_status == STREAM_CONNECT) {
        if (connect(stream_fd,
                    (struct sockaddr*)&stream_peer_sockaddr,
                    stream_peer_addr_length) < 0) {
            STREAM_PERROR("connect");
            sleep(1);
            goto STREAM_RECONNECT_LOOP;

        } else {

            stream_status = STREAM_CONNECT;
            stream_get_peer_sockaddr();
            return true;

        }

    } else {

        return false;

    }
}

bool
stream::stream_connect(const std::string& dest_host, const std::string& dest_port)
{
    int error;
    struct addrinfo *res;
    struct addrinfo hints;

    if (stream_family == 0) {
        return false;
    }

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = stream_family;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_ALL | AI_ADDRCONFIG;

    error = getaddrinfo(dest_host.c_str(), dest_port.c_str(), &hints, &res);
    if (error) {
#ifdef DEBUG
        if (debug) {
        fprintf(stderr, "%s(%d):getaddrinfo: %s\n", __FILE__, __LINE__, gai_strerror(error));
        }
#endif
        return false;
    }

    /*
    // non-block
    if (0) {
        fcntl(sock, F_SETFL, O_NONBLOCK);
    }
    */

    if (connect(stream_fd, res->ai_addr, res->ai_addrlen) < 0) {
        if(errno == EINPROGRESS) {

            // non-block
            return true;

        } else {

            STREAM_PERROR("connect");
            stream_close();
            return false;

        }

    } else {

        freeaddrinfo(res);
        stream_status = STREAM_CONNECT;
        stream_get_peer_sockaddr();
        return true;

    }

    freeaddrinfo(res);
    stream_close();
    return false;
}

bool
stream::stream_connect(const char* dest_host, const char* dest_port)
{
    const std::string str_dest_host = std::string(dest_host);
    const std::string str_dest_port = std::string(dest_port);
    return stream_connect(str_dest_host, str_dest_port);
}

bool
stream::stream_connect(const struct sockaddr* dest_addr, socklen_t dest_length)
{
    if (dest_length > sizeof(struct sockaddr_storage)) { return false; } 
    /*
    // non-block
    if (0) {
        fcntl(sock, F_SETFL, O_NONBLOCK);
    }
    */
    if (connect(stream_fd, dest_addr, dest_length) < 0) {
        if(errno == EINPROGRESS) {

            // non-block
            stream_status = STREAM_CONNECT;
            return true;

        } else {

            STREAM_PERROR("connect");
            stream_close();
            return false;

        }

    } else {

        stream_status = STREAM_CONNECT;
        stream_get_peer_sockaddr();
        return true;

    }

    stream_close();
    return false;
}

bool
stream::stream_listen(const std::string& host, const std::string& port)
{
    int error;
    struct addrinfo *res;
    struct addrinfo hints;

    if (stream_family == 0) {
        return false;
    }

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = stream_family;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    error = getaddrinfo(host.c_str(), port.c_str(), &hints, &res);
    if (error) {
#ifdef DEBUG
        if (debug) {
        fprintf(stderr, "%s(%d):getaddrinfo: %s\n", __FILE__, __LINE__, gai_strerror(error));
        }
#endif
        return false;
    }

    if (bind(stream_fd, res->ai_addr, res->ai_addrlen) < 0) {
        STREAM_PERROR("bind");
        stream_close();
        return false;
    }

    if (listen(stream_fd, SOMAXCONN) < 0) {
        STREAM_PERROR("bind");
        stream_close();
        return false;
    }

    stream_status = STREAM_LISTEN;
    return true;
}

int
stream::stream_accept()
{
    stream_accept_addr_length = sizeof(stream_accept_sockaddr);
    //memset(&stream_accept_sockaddr, 0, sizeof(stream_accept_sockaddr));

    stream_accept_fd = accept(stream_fd, 
                              (struct sockaddr*)&stream_accept_sockaddr,
                              &stream_accept_addr_length);

    if (stream_accept_fd < 0) {
        STREAM_PERROR("accept");
    }

    return stream_accept_fd;
}


struct sockaddr* stream::stream_get_peer_sockaddr()
{
    if (stream_status == STREAM_CONNECT) {

        stream_peer_addr_length = sizeof(stream_peer_sockaddr);
        if (getpeername(stream_fd, (struct sockaddr*)&stream_peer_sockaddr, &stream_peer_addr_length) < 0) {
            STREAM_PERROR("getpeername");
            return NULL;

        }

        return (struct sockaddr*)&stream_peer_sockaddr;

    } else if (stream_status == STREAM_LISTEN) {

        stream_peer_addr_length = sizeof(stream_peer_sockaddr);
        if (getpeername(stream_accept_fd, 
                        (struct sockaddr*)&stream_peer_sockaddr,
                        &stream_peer_addr_length) < 0) {
            STREAM_PERROR("getpeername");
            return NULL;
        }
        return (struct sockaddr*)&stream_peer_sockaddr;
    } else {
        return NULL;
    }
}

std::string
stream::stream_get_peer_host()
{
    stream::stream_get_peer_sockaddr();
    memset(stream_buf, 0, BUFSIZ);
    int error = 0;
    error = getnameinfo((const sockaddr*)&stream_peer_sockaddr, sizeof(stream_peer_sockaddr),
                stream_buf, BUFSIZ, NULL, 0,  NI_NUMERICHOST);
    if (error != 0) {
#ifdef DEBUG
        if (debug) {
        fprintf(stderr, "%s(%d):getnameinfo: %s\n", __FILE__, __LINE__, gai_strerror(error));
        //fprintf(stderr, "%d\n", error);
        }
#endif
        return std::string("(null)");
    }
    return std::string(stream_buf);
}

std::string
stream::stream_get_peer_port()
{
    stream::stream_get_peer_sockaddr();
    memset(stream_buf, 0, BUFSIZ);
    if (stream_family == AF_INET) {
        sprintf(stream_buf, "%hu", ntohs(((struct sockaddr_in*)&stream_peer_sockaddr)->sin_port));
        return std::string(stream_buf);
    } else if (stream_family == AF_INET6) {
        sprintf(stream_buf, "%hu", ntohs(((struct sockaddr_in6*)&stream_peer_sockaddr)->sin6_port));
        return std::string(stream_buf);
    } else {
        return std::string("(null)");
    }
    return std::string(stream_buf);
}

int
stream::stream_open(const char* type)
{
    std::string str_type = type;
    return stream_open(str_type);
}

int
stream::stream_open(const std::string& type)
{
    if (type.compare("AF_INET") == 0) {

        stream_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (stream_fd == -1) {
            STREAM_PERROR("socket");
            return -1;
        }
        stream_family = AF_INET;

    } else if (type.compare("udp4") == 0) {

        stream_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (stream_fd == -1) {
            STREAM_PERROR("socket");
            return -1;
        }
        stream_family = AF_INET;

    } else if (type.compare("UDP4") == 0) {

        stream_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (stream_fd == -1) {
            STREAM_PERROR("socket");
            return -1;
        }
        stream_family = AF_INET;

    } else if (type.compare("AF_INET6") == 0) {

        stream_fd = socket(AF_INET6, SOCK_STREAM, 0);
        if (stream_fd == -1) {
            STREAM_PERROR("socket");
            return -1;
        }
        stream_family = AF_INET6;

    } else if (type.compare("udp6") == 0) {

        stream_fd = socket(AF_INET6, SOCK_STREAM, 0);
        if (stream_fd == -1) {
            STREAM_PERROR("socket");
            return -1;
        }
        stream_family = AF_INET6;

    } else if (type.compare("UDP6") == 0) {

        stream_fd = socket(AF_INET6, SOCK_STREAM, 0);
        if (stream_fd == -1) {
            STREAM_PERROR("socket");
            return -1;
        }
        stream_family = AF_INET6;

    } else {

        return -1;

    }

    int on = 1;
    if (setsockopt(stream_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on)) < 0) {
        STREAM_PERROR("setsockopt");
        stream_close();
        return -1;
    }

    /*
    int opt;
    socklen_t optlen = sizeof(opt);
    if (getsockopt(stream_fd, SOL_SOCKET, SO_RCVBUF, (void *)&opt, &optlen) < 0) {
        STREAM_PERROR("setsockopt");
        close(stream_fd);
        stream_fd = -1;
    } else {
        //printf("REVBUF:%d\n", opt);
        ;
    }

    if (getsockopt(stream_fd, SOL_SOCKET,  SO_SNDBUF, (void *)&opt, &optlen) < 0) {
        STREAM_PERROR("setsockopt");
        close(stream_fd);
        stream_fd = -1;
    } else {
        //printf("SNDBUF:%d\n", opt);
        ;
    }
    */

    return stream_fd;
}

int stream::get_fd()
{
    return stream_fd;
}


#endif
