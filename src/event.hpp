#ifndef __EVENT_HPP__
#define __EVENT_HPP__

#include <arpa/inet.h>
#include <event2/event.h>

#include "common.hpp"
#include "id_table.hpp"
#include "stream.hpp"
#include "dgram.hpp"
#include "ns_parser.hpp"


class ev_stream : public stream {
public:
    ev_stream();
    virtual ~ev_stream();

    void es_set_ev_base(event_base *ev_base);
    void set_opposite_dgram(dgram* dgram4, dgram* dgram6);
    void set_id_table(id_table* t);
    int es_dispatch();
    int es_redispatch();

private:
    int odd_length;
    char odd_buf[BUFSIZ];
    char recv_buf[0xFFFF-40];
    id_table* table;
    dgram* udp4;
    dgram* udp6;
    event_base* es_ev_base;
    event* es_ev;
    friend void es_callback(evutil_socket_t fd, int16_t what, void* arg);
};

class ev_dgram : public dgram {
public:
    ev_dgram();
    virtual ~ev_dgram();

    void ed_set_ev_base(event_base *ev_base);
    void set_opposite_stream(ev_stream* stream);
    void set_id_table(id_table* t);
    int ed_dispatch();

private:
    char recv_buf[0xFFFF-20];
    id_table* table;
    ev_stream* tcp;
    event* ed_ev;
    event_base* ed_ev_base;
    friend void ed_callback(evutil_socket_t fd, int16_t what, void* arg);
};

void
ed_callback(evutil_socket_t fd, int16_t what, void* arg)
{
    ev_dgram* ed_ptr = (ev_dgram*)arg;

    ssize_t ret_size;
    size_t send_size;
    uint16_t dns_id;
    char* buf;
    uint16_t* msg_length;

    ret_size = ed_ptr->dgram_recvfrom((ed_ptr->recv_buf)+2, sizeof(ed_ptr->recv_buf)-2);
    if (ret_size < 0) {
        return;
    }

    buf = ed_ptr->recv_buf;
    msg_length = (uint16_t*)buf;
    send_size = (ssize_t)(ret_size + 2);
    *msg_length = htons(ret_size);
    dns_id = (*(uint16_t*)&buf[2]);

    if (debug) {
        ns_parser nsp;
        nsp.set_msg(buf+2, ret_size);
        nsp.print_query();
    }

    ed_ptr->table->set_sockaddr(dns_id, 
            ed_ptr->dgram_get_from_sockaddr(), sizeof(struct sockaddr_storage));

    if (ed_ptr->tcp->get_fd() <= 0) {
ed_callback_reconn_loop:

#ifdef __linux__

        if (tfo) {

            ret_size = ed_ptr->tcp->stream_re_sendto(buf, send_size);
            if (ret_size <= 0) {
                sleep(1);
                goto ed_callback_reconn_loop;
            }
            ed_ptr->tcp->es_redispatch();

        } else {

            ed_ptr->tcp->stream_reconnect();
            ret_size = ed_ptr->tcp->stream_send(buf, send_size, 0);
            if (ret_size < 0) {
                sleep(1);
                goto ed_callback_reconn_loop;
            }
            ed_ptr->tcp->es_redispatch();

        }

#else

        ed_ptr->tcp->stream_reconnect();
        ret_size = ed_ptr->tcp->stream_send(buf, send_size, 0);
        if (ret_size < 0) {
            sleep(1);
            goto ed_callback_reconn_loop;
        }
        ed_ptr->tcp->es_redispatch();

#endif
        return;

    } else {

        ret_size = ed_ptr->tcp->stream_send(buf, send_size, 0);

        if (ret_size < 0) {
            goto ed_callback_reconn_loop;
        }

        return;

    }

    return;
}

void
es_callback(evutil_socket_t fd, int16_t what, void* arg)
{
    ev_stream* es_ptr = (ev_stream*)arg;

    ssize_t ret_size = 0;

    char* buf;
    struct sockaddr* sa;

    uint16_t dns_id;
    uint16_t msg_length = 0;

    int seek;
    int odd_length = es_ptr->odd_length;

    memset(es_ptr->recv_buf, 0, sizeof(es_ptr->recv_buf));

    ret_size = es_ptr->stream_recv(es_ptr->recv_buf, sizeof(es_ptr->recv_buf));
    if (ret_size < 0) {
        STREAM_PERROR("recv");
        es_ptr->odd_length = 0;
        memset(es_ptr->recv_buf, 0, sizeof(es_ptr->recv_buf));
        memset(es_ptr->odd_buf, 0, sizeof(es_ptr->odd_buf));
        return;
    } else if (ret_size == 0) {
        return;
    }

    //if (one) { es_ptr->stream_close(); }

    if (odd_length) {
        memmove((es_ptr->recv_buf)+odd_length, es_ptr->recv_buf, ret_size);
        memcpy(es_ptr->recv_buf, es_ptr->odd_buf, odd_length);
        ret_size = ret_size + odd_length;
    }

    seek = 0;
    buf = es_ptr->recv_buf;

    for (;;) {
        if (ret_size == seek + 0) {
#ifdef DEBUG
            // XXX be still thinking
            if (debug) printf("odd_buffering1(%d)\n", __LINE__);
            if (debug) printf("exit(cant implementation)");
#endif
            return;

        } else if (ret_size == seek + 1) {

#ifdef DEBUG
            // XXX be still thinking
            if (debug) printf("odd_buffering1(%d)\n", __LINE__);
            if (debug) printf("exit(cant implementation)");
#endif
            odd_length = 1;
            memcpy(es_ptr->odd_buf, &buf[seek+0], odd_length);
            return;

        } else if (ret_size == seek + 2) {
#ifdef DEBUG
            // XXX be still thinking
            if (debug) printf("odd_buffering1(%d)\n", __LINE__);
            if (debug) printf("exit(cant implementation)");
#endif
            odd_length = 2;
            memcpy(es_ptr->odd_buf, &buf[seek+0], odd_length);
            return;
        }

        msg_length = ntohs(*(uint16_t*)&buf[seek]);

        if (ret_size < seek + 2 + msg_length) {
#ifdef DEBUG
            if (debug) printf("odd_buffering2(%d)\n", __LINE__);
#endif
            odd_length = ret_size - seek - 2;
            memcpy(es_ptr->odd_buf, &buf[seek+2], odd_length);
            return;
        }
        dns_id = (*(uint16_t*)&buf[seek+2]);

        sa = (struct sockaddr*)es_ptr->table->get_sockaddr(dns_id);
        if (sa == NULL) {
            return;
        }

        if (debug) {
            ns_parser nsp;
            nsp.set_msg(&buf[seek+2], msg_length);
            nsp.print_answer();
        }

        if (sa->sa_family == AF_INET) {
            es_ptr->udp4->dgram_set_to(sa, sizeof(struct sockaddr_in));
            es_ptr->udp4->dgram_sendto(&buf[seek+2], msg_length, 0);
        } else if (sa->sa_family == AF_INET6) {
            es_ptr->udp6->dgram_set_to(sa, sizeof(struct sockaddr_in6));
            es_ptr->udp6->dgram_sendto(&buf[seek+2], msg_length, 0);
        }

        if (ret_size > seek + 2 + msg_length) {
#ifdef DEBUG
            if (debug) {
                printf("ret_size   : %lu\n", ret_size);
                printf("seeker_size: %d\n", seek+2+msg_length);
                printf("continue\n");
            }
#endif
            seek = seek + msg_length + 2;
            continue;
        } else {
            break;
        }

    }

    odd_length = 0;
    if (one) { es_ptr->stream_close(); }
    return;
}

ev_dgram::ev_dgram()
{
    ed_ev_base = NULL;
    ed_ev      = NULL;
    tcp        = NULL;
    table      = NULL;
    memset(recv_buf, 0, sizeof(recv_buf));
}

ev_dgram::~ev_dgram()
{
}

void
ev_dgram::ed_set_ev_base(event_base *ev_base)
{
    ed_ev_base = ev_base;
    return;
} 

void
ev_dgram::set_opposite_stream(ev_stream* stream)
{
    tcp = stream;
    return;
}

void
ev_dgram::set_id_table(id_table* t)
{
    table = t;
    return;
}


int
ev_dgram::ed_dispatch()
{
    if (get_fd() < 0) {
        fprintf(stderr, "%s(%d): couldn't open dgram socket\n", __FILE__, __LINE__);
        return -1;
    }

    ed_ev = event_new(ed_ev_base, get_fd(), EV_READ | EV_PERSIST, ed_callback, this);
    if (!ed_ev) {
        fprintf(stderr, "%s(%d): couldn't create new event\n", __FILE__, __LINE__);
        return -1;
    }

    return event_add(ed_ev, NULL);
}


ev_stream::ev_stream()
{
    udp4       = NULL;
    udp6       = NULL;
    es_ev_base = NULL;
    es_ev      = NULL;
    table      = NULL;
    odd_length = 0;
    memset(odd_buf, 0, sizeof(odd_buf));
    memset(recv_buf, 0, sizeof(recv_buf));
}

ev_stream::~ev_stream()
{
}

void
ev_stream::es_set_ev_base(event_base *ev_base)
{
    es_ev_base = ev_base;
    return;
} 

void
ev_stream::set_opposite_dgram(dgram* dgram4, dgram* dgram6)
{
    udp4 = dgram4;
    udp6 = dgram6;
}

void
ev_stream::set_id_table(id_table* t)
{
    table = t;
    return;
}

int
ev_stream::es_redispatch()
{
    event_del(es_ev);
    free(es_ev);
    return es_dispatch();
}

int ev_stream::es_dispatch()
{
    if (get_fd() < 0) {
        fprintf(stderr, "%s(%d) couldn't open stream socket\n", __FILE__, __LINE__);
        return -1;
    }

    es_ev = event_new(es_ev_base, get_fd(), EV_READ | EV_PERSIST, es_callback, this);
    if (!es_ev) {
        fprintf(stderr, "%s(%d) couldn't create new event\n", __FILE__, __LINE__);
        return -1;
    }

    return event_add(es_ev, NULL);
}

#endif
