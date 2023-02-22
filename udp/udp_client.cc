#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Log.hpp"
#include <iostream>
#include <strings.h>
#include <cstring>
#include "Thread.hpp"
#include <memory>
#include <unistd.h>

std::string ip;
uint16_t port = 0;

void usage()
{
    std::cout << "Usage: ./udp_client ip port\n";
}
static void *sender_fun(void *args)
{
    ThreadData *tData = static_cast<ThreadData *>(args);
    int sock = *(int *)(tData->args);
    std::string name = tData->name;
    // 初始化socket结构体并填充对应字段
    struct sockaddr_in socketAddr;
    bzero(&socketAddr, sizeof(socketAddr));
    socketAddr.sin_addr.s_addr = inet_addr(ip.c_str());
    socketAddr.sin_family = AF_INET;
    socketAddr.sin_port = htons(port);
    // 当client第一次发消息给server时会自动绑定socket
    std::string message;
    while (true)
    {
        std::cerr << "请输入消息: ";
        getline(std::cin, message);
        if (strcmp(message.c_str(), "quit") == 0)
        {
            break;
        }
        sendto(sock, message.c_str(), message.size(), 0, (struct sockaddr *)&socketAddr, sizeof(socketAddr));
    }
    return nullptr;
}
static void *recever_fun(void *args)
{
    char buffer[1024] = {'\0'};

    ThreadData *tData = static_cast<ThreadData *>(args);
    int sock = *(int *)(tData->args);
    std::string name = tData->name;
    while (true)
    {
        struct sockaddr_in temp;
        socklen_t socklen;
        ssize_t n = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&temp, &socklen);
        if (n > 0)
        {
            buffer[n] = '\0';
            std::cout << buffer << std::endl;
            fflush(stdout);
        }
    }
}
int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        usage();
        exit(3);
    }
    ip = argv[1];
    port = atoi(argv[2]);
    // 创建socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        std::cerr << "socket error" << std::endl;
        exit(2);
    }
    // 创建多线程
    std::unique_ptr<Thread> sender(new Thread(1, sender_fun, &sock));
    std::unique_ptr<Thread> recever(new Thread(2, recever_fun, &sock));

    sender->start();
    recever->start();

    sender->join();
    recever->join();

    close(sock);
    // // 初始化socket结构体并填充对应字段
    // struct sockaddr_in socketAddr;
    // bzero(&socketAddr, sizeof(socketAddr));
    // socketAddr.sin_addr.s_addr = inet_addr(argv[1]);
    // socketAddr.sin_family = AF_INET;
    // socketAddr.sin_port = htons(atoi(argv[2]));
    // // 当client第一次发消息给server时会自动绑定socket
    // char buffer[1024] = {'\0'};
    // while (true)
    // {
    //     std::cout << "请输入指令: ";
    //     std::string message;
    //     getline(std::cin, message);
    //     if (strcmp(message.c_str(), "quit") == 0)
    //     {
    //         break;
    //     }
    //     ssize_t ret = sendto(sock, message.c_str(), message.size(), 0, (struct sockaddr *)&socketAddr, sizeof(socketAddr));

    //     struct sockaddr_in temp;
    //     socklen_t socklen;
    //     ssize_t n = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&temp, &socklen);
    //     if (n > 0)
    //     {
    //         buffer[n] = '\0';
    //         logMessage(NORMAL, "%s-%d send message: %s", inet_ntoa(temp.sin_addr), ntohs(temp.sin_port), buffer);
    //     }
    // }
    logMessage(NORMAL, "客户端退出");
    return 0;
}