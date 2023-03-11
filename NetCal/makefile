.PHONY:all
all:CalClient CalServer

CalClient:CalClient.cc
	g++ -o $@ $^ -std=c++11 -ljsoncpp
CalServer:CalServer.cc
	g++ -o $@ $^ -std=c++11 -lpthread -ljsoncpp

.PHONY:clean
clean:
	rm -f CalClient CalServer