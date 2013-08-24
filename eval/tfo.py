#!/usr/bin/env python

import sys
import socket
import time

PORT = 10443

TCP_FASTOPEN = 23
MSG_FASTOPEN = 0x20000000

def server():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.setsockopt(socket.SOL_TCP, TCP_FASTOPEN, 5)

    s.bind(("", PORT))
    s.listen(socket.SOMAXCONN)

    while True:
        conn, addr = s.accept()
        while True:
            ret = conn.recv(1024)
            if len(ret) <= 0:
                break
            print ret;
            conn.sendall(ret)
        conn.close()
    s.close()


def normal_client():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    p = time.time()
    s.connect((sys.argv[2], PORT))
    s.sendto("1. Using TCP fast open!!!")
    ret = s.recv(1024)
    c = time.time()
    print (c - p)
    print ret

    s.send("2. continued data sending")
    ret = s.recv(1024)
    print ret

    s.close()

def tfo_client():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    p = time.time()
    s.sendto("1. Using TCP fast open!!!", MSG_FASTOPEN, (sys.argv[2], PORT))
    ret = s.recv(1024)
    c = time.time()
    print (c - p)
    print ret

    s.send("2. continued data sending")
    ret = s.recv(1024)
    print ret

    s.close()

def usage():
    print 'Usage:'
    print ' python tfo.py [option]'
    print ''
    print '   -s'
    print '     server mode'
    print ''
    print '   -n [servername]'
    print '     normal clinet mode'
    print ''
    print '   -f [servername]'
    print '     fastopen clinet mode'
    print ''

def main():
    if '-s' in sys.argv:
        server()
    elif '-n' in sys.argv:
        normal_client()
    elif '-f' in sys.argv:
        tfo_client()
    else:
        usage()

if __name__ == '__main__':
    main()
