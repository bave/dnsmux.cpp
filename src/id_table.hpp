#ifndef __ID_TABLE_HPP__
#define __ID_TABLE_HPP__

#include <sys/socket.h>
#include <netinet/in.h>

#include <stdlib.h>
#include <string.h>

#include <map>

#include "common.hpp"

class it_content
{
public:
    uint16_t id;
    struct sockaddr_storage ss;
    it_content() {
        id = 0;
        memset(&ss, 0, sizeof(ss));
    };
    virtual ~it_content() {};
};



#ifdef CPP11
#include <memory>
typedef std::shared_ptr<it_content> ptr_it_content;
#else
#include <boost/shared_ptr.hpp>
typedef boost::shared_ptr<it_content> ptr_it_content;
#endif



class id_table {
public:
    id_table();
    virtual ~id_table();

    struct sockaddr_storage* get_sockaddr(uint16_t id);
    struct in_addr* get_addr4(uint16_t id);
    struct in6_addr* get_addr6(uint16_t id);
    uint16_t get_port(uint16_t id);
    uint8_t get_family(uint16_t id);

    bool set_sockaddr(uint16_t id, struct sockaddr* sa, size_t salen);
    bool set_family(uint16_t id, uint8_t family);
    bool set_port(uint16_t id, uint16_t port);
    bool set_port(uint16_t id, std::string& port);
    bool set_addr4(uint16_t id, struct in_addr  addr);
    bool set_addr6(uint16_t id, struct in6_addr addr);

private:
    std::map<uint16_t, ptr_it_content> table;
};

id_table::id_table()
{
    int i;
    for (i=0; i<=0xFFFF; i++) {
        table[i] = ptr_it_content(new it_content);
        table[i]->id = i;
    }
}

id_table::~id_table()
{
}

struct sockaddr_storage*
id_table::get_sockaddr(uint16_t id)
{
    return &(table[id]->ss);
}

struct in_addr*
id_table::get_addr4(uint16_t id)
{
    uint8_t f = get_family(id);
    if (f != AF_INET) {
        return NULL;
    }
    struct sockaddr_storage* ss = get_sockaddr(id);
    if (ss != NULL) {
        return &(((struct sockaddr_in*)ss)->sin_addr);
    } else {
        return NULL;
    }
}

struct in6_addr*
id_table::get_addr6(uint16_t id)
{
    uint8_t f = get_family(id);
    if (f != AF_INET6) {
        return NULL;
    }
    struct sockaddr_storage* ss = get_sockaddr(id);
    if (ss != NULL) {
        return &(((struct sockaddr_in6*)ss)->sin6_addr);
    } else {
        return NULL;
    }
}

uint16_t
id_table::get_port(uint16_t id)
{
    uint8_t f = get_family(id);
    struct sockaddr_storage* ss = get_sockaddr(id);
    if (f == AF_INET) {
        return ((struct sockaddr_in*)ss)->sin_port;
    } else if (f == AF_INET6){
        return ((struct sockaddr_in6*)ss)->sin6_port;
    } else {
        return 0;
    }
}

uint8_t
id_table::get_family(uint16_t id)
{
    return table[id]->ss.ss_family;
}

bool id_table::set_sockaddr(uint16_t id, struct sockaddr* sa, size_t salen)
{
    struct sockaddr_storage* ss = get_sockaddr(id);
    if (ss == NULL) {
        return false;
    } else {
        memcpy(ss, sa, salen);
        return true;
    }
}

bool
id_table::set_family(uint16_t id, uint8_t family)
{
    struct sockaddr_storage* ss = get_sockaddr(id);
    if (ss == NULL) {
        return false;
    }
    ss->ss_family = family;
    return true;
}

bool
id_table::set_port(uint16_t id, uint16_t port)
{
    struct sockaddr_storage* ss = get_sockaddr(id);
    if (ss == NULL) {
        return false;
    }
    uint8_t f = ss->ss_family;
    if (f == AF_INET) {
        ((struct sockaddr_in*)ss)->sin_port = port;
        return true;
    } if (f == AF_INET6) {
        ((struct sockaddr_in6*)ss)->sin6_port = port;
        return true;
    } else {
        return false;
    }
}

bool
id_table::set_port(uint16_t id, std::string& port)
{
    return set_port(id, atoi(port.c_str()));
}

bool
id_table::set_addr4(uint16_t id, struct in_addr  addr)
{
    struct sockaddr_storage* ss = get_sockaddr(id);
    if (ss == NULL) {
        return false;
    }
    uint8_t f = ss->ss_family;
    if (f == AF_INET) {
        ((struct sockaddr_in*)ss)->sin_addr = addr;
        return true;
    } if (f == AF_INET6) {
        return false;
    } else {
        return false;
    }
}

bool
id_table::set_addr6(uint16_t id, struct in6_addr addr)
{
    struct sockaddr_storage* ss = get_sockaddr(id);
    if (ss == NULL) {
        return false;
    }
    uint8_t f = ss->ss_family;
    if (f == AF_INET) {
        return false;
    } if (f == AF_INET6) {
        ((struct sockaddr_in6*)ss)->sin6_addr = addr;
        return true;
    } else {
        return false;
    }
    return false;
}

#endif
