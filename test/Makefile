all:dgram_server dgram_client stream_server stream_client id_table



dgram_server: dgram_server.cpp ../src/dgram.hpp
	g++ -g -Wall -o ./dgram_server ./dgram_server.cpp -DDEBUG

dgram_client: dgram_client.cpp ../src/dgram.hpp
	g++ -g -Wall -o ./dgram_client ./dgram_client.cpp -DDEBUG

stream_server: stream_server.cpp ../src/stream.hpp
	g++ -g -Wall -o ./stream_server ./stream_server.cpp -DDEBUG

stream_client: stream_client.cpp ../src/stream.hpp
	g++ -g -Wall -o ./stream_client ./stream_client.cpp -DDEBUG

id_table: id_table.cpp ../src/id_table.hpp
	#clang++ -std=c++11 -stdlib=libc++ -o ./id_table id_table.cpp
	g++ -g -Wall -o ./id_table ./id_table.cpp -DDEBUG

clean:
	rm -rf *.dSYM ./stream_client ./stream_server ./dgram_server ./dgram_client ./id_table
