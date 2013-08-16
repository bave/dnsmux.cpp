dnsmux.cpp
========

 * The dnsmux.cpp is DNS Proxy which use 1 tcp session multiplexing dns querys.

```
dnsmux.cpp depends on:
        libevent (http://www.monkey.org/~provos/libevent/)
        boost (http://www.boost.org/)

# git clone https://github.com/bave/dnsmux.cpp.git
# cd dnsmux.cpp/src
# make
# ./dnsmux
```

```
Usage: node dnsmux [options]
show help command
    -h, --help
show debug messages
    -d, --debug
one query use one tcp sesstion
    -1, --one
proxy service port (default: 53)
    -l, --local <value>
dns server port (default: 53)
    -p, --port <value>
dns server ip (default: 8.8.8.8)
    -s, --server <value>
show version of dnsmux.cpp
    -v, --version
```

## Contributing

1. Fork it
2. Create your feature branch (`git checkout -b new-branch-name`)
3. Commit your changes (`git commit -am 'Add comment at some your new features'`)
4. Push to the branch (`git push origin new-branch-name`)
5. Create new Pull Request


