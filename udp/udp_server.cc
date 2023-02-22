#include "udp_server.hpp"
#include <iostream>
#include <memory>
void usage()
{
    std::cout << "Usage: ./udp_server port\n";
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        usage();
        exit(3);
    }
    // 创建服务器
    std::unique_ptr<UdpServer>server(new UdpServer(atoi(argv[1])));
    // 初始化并启动服务器
    server->initServer();
    server->startServer();
    return 0;
}