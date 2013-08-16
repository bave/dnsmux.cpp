all: g_dnsmux

g_dnsmux: dnsmux.cpp event.hpp stream.hpp dgram.hpp id_table.hpp
	g++ -g -Wall -Wno-write-strings -o ./dnsmux ./dnsmux.cpp -levent -lresolv -DDEBUG

c_dnsmux: dnsmux++.cpp event.hpp stream.hpp dgram.hpp id_table.hpp
	clang++ -g -Wall -Wno-write-strings -std=c++11 -stdlib=libc++ -o ./dnsmux ./dnsmux.cpp -levent -lresolv -DDEBUG -DCPP11

clean:
	rm -rf dnsmux.dSYM dnsmux

