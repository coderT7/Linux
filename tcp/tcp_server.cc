#include "tcp_server.hpp"
#include <iostream>
#include <memory>
void Usage(char* args[]){
    std::cout << args[0] << " port" << std::endl;
}

int main(int argc, char* args[])
{
    if (argc != 2)
    {
        Usage(args);
        exit(0);
    }
    std::cout << "server start" << std::endl;
    std::unique_ptr<Tcp_Server> tcp_server(new Tcp_Server(atoi(args[1])));
    tcp_server->server_init();
    tcp_server->server_start();

    return 0;
}