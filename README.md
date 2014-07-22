dnsmux.cpp
========

 * The dnsmux.cpp is DNS Proxy which use 1 tcp session multiplexing dns querys.

```
dnsmux.cpp depends on:
        libevent (http://www.monkey.org/~provos/libevent/)
        boost (http://www.boost.org/)

# git clone https://github.com/bave/dnsmux.cpp.git
# cd dnsmux.cpp
# gmake
# ./dnsmux
```

```
Usage: dnsmux [options]
show help command
    -h, --help
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
enable tfo (linux only)
    -t, --tfo
show debug messages (when you compile, add -DDEBUG. (make debug))
    -d, --debug
```

## ToDO
1. レゾルバから要求されたリソースレコードが4096byteを超えるものだった場合のハンドリング
2. hoge

## Contributing

1. Fork it
2. Create your feature branch (`git checkout -b new-branch-name`)
3. Commit your changes (`git commit -am 'Add comment at some your new features'`)
4. Push to the branch (`git push origin new-branch-name`)
5. Create new Pull Request


