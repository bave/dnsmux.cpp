#ifndef __DGRAM_HPP__
#define __DGRAM_HPP__

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <err.h>
#include <unistd.h>

#include <iostream>

#include "common.hpp"

#ifdef DEBUG
#define DGRAM_PERROR(func) do {                                 \
    if (debug) {                                                \
    char s[BUFSIZ];                                             \
    memset(s, 0, BUFSIZ);                                       \
    snprintf(s, BUFSIZ, "%s(%d):%s", __FILE__, __LINE__, func); \
    perror(s);                                                  \
    }                                                           \
} while (false)
#else
#define DGRAM_PERROR(func) do {} while (false)
#endif


class dgram
{
public:

    dgram() {
        dgram_family = 0;
        dgram_fd = -1;
        memset(&dgram_to_addr, 0, sizeof(dgram_to_addr));
        memset(&dgram_from_addr, 0, sizeof(dgram_from_addr));
    };
    virtual ~dgram() {
        if (dgram_fd != -1) {
            close(dgram_fd);
        }
    };

    int dgram_open(const char* type);
    int dgram_open(const std::string& type);

    bool dgram_bind(const std::string& host, const std::string& port);
    bool dgram_bind(const char* host, const char* port);

    ssize_t dgram_sendto(const void* buf, size_t length, int flag);
    ssize_t dgram_recvfrom(void* buf, size_t length);

    bool dgram_set_to(const std::string& dest_host, const std::string& dest_port);
    bool dgram_set_to(const char* dest_host, const char* dest_port);
    bool dgram_set_to(const struct sockaddr* dest_addr, socklen_t dest_length);

    void dgram_set_to2from();
    void dgram_set_from2to();

    struct sockaddr* dgram_get_to_sockaddr();
    struct sockaddr* dgram_get_from_sockaddr();

    std::string dgram_get_to_host();
    std::string dgram_get_from_host();
 
    std::string dgram_get_to_port();
    std::string dgram_get_from_port();
       
    int get_fd();

private:
    int dgram_fd;
    int dgram_family;

    char dgram_buf[BUFSIZ];
    struct sockaddr_storage dgram_to_addr;
    struct sockaddr_storage dgram_from_addr;

    socklen_t dgram_recv_addr_length;
};

ssize_t dgram::dgram_sendto(const void* buf, size_t length, int flag)
{
    ssize_t ret;
    if (dgram_family == AF_INET) {
        ret = sendto(dgram_fd, buf, length, flag,
                (struct sockaddr*)&dgram_to_addr, sizeof(struct sockaddr_in));
        if (ret == -1) {
            DGRAM_PERROR("sendto");
        }
    } else if (dgram_family == AF_INET6) {
        ret = sendto(dgram_fd, buf, length, flag,
                (struct sockaddr*)&dgram_to_addr, sizeof(struct sockaddr_in6));
        if (ret == -1) {
            DGRAM_PERROR("sendto");
        }
    } else {
        return -1;
    }
    return ret;
}

bool dgram::dgram_bind(const std::string& host, const std::string& port)
{
    int error;
    struct addrinfo *res;
    struct addrinfo hints;

    if (dgram_family == 0) {
        return false;
    } 

    if (dgram_fd == -1) {
        return false;
    }

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = dgram_family;
    hints.ai_socktype = SOCK_DGRAM;
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

    if (bind(dgram_fd, res->ai_addr, res->ai_addrlen) < 0) {
        DGRAM_PERROR("bind");
        close(dgram_fd);
        dgram_fd = -1;
        dgram_family = 0;
        return false;
    }

    freeaddrinfo(res);
    return true;
}

bool dgram::dgram_bind(const char* host, const char* port)
{
    const std::string str_host = std::string(host);
    const std::string str_port = std::string(port);
    return dgram_bind(str_host, str_port);
}

ssize_t dgram::dgram_recvfrom(void* buf, size_t length)
{
    ssize_t ret;
    if (dgram_family == AF_INET) {
        dgram_recv_addr_length = sizeof(struct sockaddr_in);
        ret = recvfrom(dgram_fd, buf, length, 0,
                (struct sockaddr*)&dgram_from_addr, &dgram_recv_addr_length);
    } else if (dgram_family == AF_INET6) {
        dgram_recv_addr_length = sizeof(struct sockaddr_in6);
        ret = recvfrom(dgram_fd, buf, length, 0,
                (struct sockaddr*)&dgram_from_addr, &dgram_recv_addr_length);
    } else {
        return -1;
    }
    return ret;
}

bool dgram::dgram_set_to(const std::string& dest_host, const std::string& dest_port)
{
    int error;
    struct addrinfo *res;
    struct addrinfo hints;

    if (dgram_family == 0) {
        return false;
    }

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = dgram_family;
    hints.ai_socktype = SOCK_DGRAM;
#ifdef __FreeBSD__
    hints.ai_flags = 0;
#else
    hints.ai_flags = AI_ALL | AI_ADDRCONFIG;
#endif

    error = getaddrinfo(dest_host.c_str(), dest_port.c_str(), &hints, &res);
    if (error) {
#ifdef DEBUG
        if (debug) {
        fprintf(stderr, "%s(%d):getaddrinfo: %s\n", __FILE__, __LINE__, gai_strerror(error));
        }
#endif
        return false;
    }

    memcpy(&dgram_to_addr, res->ai_addr, res->ai_addrlen);
    freeaddrinfo(res);
    return true;
}

bool dgram::dgram_set_to(const char* dest_host, const char* dest_port)
{
    const std::string str_dest_host = std::string(dest_host);
    const std::string str_dest_port = std::string(dest_port);
    return dgram_set_to(str_dest_host, str_dest_port);
}

bool dgram::dgram_set_to(const struct sockaddr* dest_addr, socklen_t dest_length)
{
    if (dest_length > sizeof(struct sockaddr_storage)) { return false; } 
    memcpy(&dgram_to_addr, dest_addr, dest_length);
    return true;
}

void dgram::dgram_set_to2from()
{
    dgram_from_addr = dgram_to_addr;
    return;
}

void dgram::dgram_set_from2to()
{
    dgram_to_addr = dgram_from_addr;
    return;
}

struct sockaddr* dgram::dgram_get_to_sockaddr()
{
    return (struct sockaddr*)&dgram_to_addr;
}

struct sockaddr* dgram::dgram_get_from_sockaddr()
{
    return (struct sockaddr*)&dgram_from_addr;
}

std::string dgram::dgram_get_to_host()
{
    int error = 0;
    memset(dgram_buf, 0, BUFSIZ);
    error = getnameinfo((const sockaddr*)&dgram_to_addr, sizeof(dgram_to_addr),
                dgram_buf, BUFSIZ, NULL, 0,  NI_NUMERICHOST);
    if (error != 0) {
#ifdef DEBUG
        if (debug) {
        fprintf(stderr, "%s(%d):getnameinfo: %s\n", __FILE__, __LINE__, gai_strerror(error));
        printf("%d\n", error);
        }
#endif
        return std::string("(null)");
    }
    return std::string(dgram_buf);
}

std::string dgram::dgram_get_from_host()
{
    int error = 0;
    memset(dgram_buf, 0, BUFSIZ);
    error = getnameinfo((const sockaddr*)&dgram_from_addr, sizeof(dgram_from_addr),
                dgram_buf, BUFSIZ, NULL, 0,  NI_NUMERICHOST);
    if (error != 0) {
#ifdef DEBUG
        if (debug) {
        fprintf(stderr, "%s(%d):getnameinfo: %s\n", __FILE__, __LINE__, gai_strerror(error));
        printf("%d\n", error);
        }
#endif
        return std::string("(null)");
    }
    return std::string(dgram_buf);
}

std::string dgram::dgram_get_to_port()
{
    memset(dgram_buf, 0, BUFSIZ);
    if (dgram_family == AF_INET) {
        sprintf(dgram_buf, "%hu", ntohs(((struct sockaddr_in*)&dgram_to_addr)->sin_port));
        return std::string(dgram_buf);
    } else if (dgram_family == AF_INET6) {
        sprintf(dgram_buf, "%hu", ntohs(((struct sockaddr_in6*)&dgram_to_addr)->sin6_port));
        return std::string(dgram_buf);
    } else {
        return std::string("(null)");
    }
}

std::string dgram::dgram_get_from_port()
{
    memset(dgram_buf, 0, BUFSIZ);
    if (dgram_family == AF_INET) {
        sprintf(dgram_buf, "%hu", ntohs(((struct sockaddr_in*)&dgram_from_addr)->sin_port));
        return std::string(dgram_buf);
    } else if (dgram_family == AF_INET6) {
        sprintf(dgram_buf, "%hu", ntohs(((struct sockaddr_in6*)&dgram_from_addr)->sin6_port));
        return std::string(dgram_buf);
    } else {
        return std::string("(null)");
    }
}

int dgram::dgram_open(const char* type)
{
    std::string str_type = type;
    return dgram_open(str_type);
}

int dgram::dgram_open(const std::string& type)
{
    if (type.compare("AF_INET") == 0) {
        dgram_fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (dgram_fd == -1) {
            DGRAM_PERROR("socket");
            return -1;
        }
        dgram_family = AF_INET;
    } else if (type.compare("udp4") == 0) {
        dgram_fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (dgram_fd == -1) {
            DGRAM_PERROR("socket");
            return -1;
        }
        dgram_family = AF_INET;
    } else if (type.compare("UDP4") == 0) {
        dgram_fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (dgram_fd == -1) {
            DGRAM_PERROR("socket");
            return -1;
        }
        dgram_family = AF_INET;
    } else if (type.compare("AF_INET6") == 0) {
        dgram_fd = socket(AF_INET6, SOCK_DGRAM, 0);
        if (dgram_fd == -1) {
            DGRAM_PERROR("socket");
            return -1;
        }
        dgram_family = AF_INET6;
    } else if (type.compare("udp6") == 0) {
        dgram_fd = socket(AF_INET6, SOCK_DGRAM, 0);
        if (dgram_fd == -1) {
            DGRAM_PERROR("socket");
            return -1;
        }
        dgram_family = AF_INET6;
    } else if (type.compare("UDP6") == 0) {
        dgram_fd = socket(AF_INET6, SOCK_DGRAM, 0);
        if (dgram_fd == -1) {
            DGRAM_PERROR("socket");
            return -1;
        }
        dgram_family = AF_INET6;
    } else {
        return -1;
    }

    int on = 1;
    if (setsockopt(dgram_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on)) < 0) {
        DGRAM_PERROR("setsockopt");
        close(dgram_fd);
        dgram_fd = -1;
        dgram_family = 0;
        return -1;
    }

    /*
    // recvbuffer..
    int opt;
    socklen_t optlen = sizeof(opt);
    if (getsockopt(dgram_fd, SOL_SOCKET, SO_RCVBUF, (void *)&opt, &optlen) < 0) {
        DGRAM_PERROR("setsockopt");
        close(dgram_fd);
        dgram_fd = -1;
        dgram_family = 0;
    } else {
        //printf("REVBUF:%d\n", opt);
        ;
    }

    if (getsockopt(dgram_fd, SOL_SOCKET,  SO_SNDBUF, (void *)&opt, &optlen) < 0) {
        DGRAM_PERROR("setsockopt");
        close(dgram_fd);
        dgram_fd = -1;
        dgram_family = 0;
    } else {
        //printf("SNDBUF:%d\n", opt);
        ;
    }
    */

    return dgram_fd;
}

int dgram::get_fd()
{
    return dgram_fd;
}

#endif
