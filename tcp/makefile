.PHONY:all
all:tcp_client tcp_server

tcp_client:tcp_client.cc
	g++ -o $@ $^ -lpthread -std=c++11	
tcp_server:tcp_server.cc
	g++ -o $@ $^ -lpthread -std=c++11 -fpermissive

.PHONY:clean
clean:
	rm -rf tcp_client tcp_server	