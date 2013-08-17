

UNAME := $(shell uname -s)

all: dnsmux
.PHONY : all

ifeq ($(UNAME),Darwin)

dnsmux: dnsmux.cpp event.hpp stream.hpp dgram.hpp id_table.hpp ns_parser.hpp utils.hpp common.hpp
	clang++ -O3 -Wno-write-strings -std=c++11 -stdlib=libc++ -o $@ ./dnsmux.cpp -levent -lresolv -DDEBUG -DCPP11
.PHONY : dnsmux

debug: dnsmux.cpp event.hpp stream.hpp dgram.hpp id_table.hpp ns_parser.hpp utils.hpp common.hpp
	clang++ -O0 -g -Wall -Wno-write-strings -std=c++11 -stdlib=libc++ -o dnsmux_debug ./dnsmux.cpp -levent -lresolv -DDEBUG -DCPP11
.PHONY : debug

else

dnsmux: dnsmux.cpp event.hpp stream.hpp dgram.hpp id_table.hpp ns_parser.hpp utils.hpp common.hpp
	g++ -O3 -Wall -Wno-write-strings -o $@ ./dnsmux.cpp -levent -lresolv -DDEBUG
.PHONY : dnsmux

debug: dnsmux.cpp event.hpp stream.hpp dgram.hpp id_table.hpp ns_parser.hpp utils.hpp common.hpp
	g++ -O0 -g -Wall -Wno-write-strings -o dnsmux_debug ./dnsmux.cpp -levent -lresolv -DDEBUG
.PHONY : debug

endif


clean:
	rm -rf dnsmux.dSYM dnsmux dnsmux_debug dnsmux_debug.dSYM
.PHONY : clean

