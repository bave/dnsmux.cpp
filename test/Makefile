
UNAME := $(shell uname -s)

ifeq ($(UNAME),Darwin)

all:dgram_server dgram_client stream_server stream_client id_table name_pkt name_tcp_pkt

dgram_server: dgram_server.cpp ../src/dgram.hpp
	g++ -g -Wall -o ./dgram_server ./dgram_server.cpp -DDEBUG

dgram_client: dgram_client.cpp ../src/dgram.hpp
	g++ -g -Wall -o ./dgram_client ./dgram_client.cpp -DDEBUG

stream_server: stream_server.cpp ../src/stream.hpp
	g++ -g -Wall -o ./stream_server ./stream_server.cpp -DDEBUG

stream_client: stream_client.cpp ../src/stream.hpp
	g++ -g -Wall -o ./stream_client ./stream_client.cpp -DDEBUG

id_table: id_table.cpp ../src/id_table.hpp
	g++ -g -Wall -o ./id_table ./id_table.cpp -DDEBUG

name_pkt: name_pkt.cpp name_pkt.hpp
	g++ -g -Wall -o ./name_pkt ./name_pkt.cpp -lresolv

name_tcp_pkt: name_tcp_pkt.cpp name_pkt.hpp
	g++ -g -Wall -o ./name_tcp_pkt ./name_tcp_pkt.cpp -lresolv

poisoning_a: name_pkt_poison.cpp name_pkt.hpp
	g++ -g -Wall -o ./poisoning_a ./name_pkt_poison.cpp -lresolv

else
ifeq ($(UNAME),Linux)

all:dgram_server dgram_client stream_server stream_client id_table name_pkt name_tcp_pkt name_tfo_pkt

dgram_server: dgram_server.cpp ../src/dgram.hpp
	g++ -g -Wall -o ./dgram_server ./dgram_server.cpp -DDEBUG

dgram_client: dgram_client.cpp ../src/dgram.hpp
	g++ -g -Wall -o ./dgram_client ./dgram_client.cpp -DDEBUG

stream_server: stream_server.cpp ../src/stream.hpp
	g++ -g -Wall -o ./stream_server ./stream_server.cpp -DDEBUG

stream_client: stream_client.cpp ../src/stream.hpp
	g++ -g -Wall -o ./stream_client ./stream_client.cpp -DDEBUG

id_table: id_table.cpp ../src/id_table.hpp
	g++ -g -Wall -o ./id_table ./id_table.cpp -DDEBUG

name_pkt: name_pkt.cpp name_pkt.hpp
	g++ -g -Wall -o ./name_pkt ./name_pkt.cpp -lresolv

name_tcp_pkt: name_tcp_pkt.cpp name_pkt.hpp
	g++ -g -Wall -o ./name_tcp_pkt ./name_tcp_pkt.cpp -lresolv

name_tfo_pkt: name_tfo_pkt.cpp name_pkt.hpp
	g++ -g -Wall -o ./name_tfo_pkt ./name_tfo_pkt.cpp -lresolv

else
all:
	echo $(uname)

endif
endif

clean:
	rm -rf *.dSYM ./stream_client ./stream_server ./dgram_server ./dgram_client ./id_table ./name_pkt ./name_tcp_pkt ./name_tfo_pkt ./poisoning_a
