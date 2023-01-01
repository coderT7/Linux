.PHONY:all
all:cline myserver

cline:cline.cc
	@g++ -o $@ $^ -std=c++11
myserver:server.cc
	@g++ -o $@ $^ -std=c++11

.PHONY:clean
clean:
	@rm -rf cline myserver