oj_server:oj_server.cc
	g++ -o $@ $^ -std=c++11 -I./include -L./lib -ljsoncpp -lpthread -lctemplate -lmysqlclient

.PHONY:clean
clean:
	rm -f oj_server