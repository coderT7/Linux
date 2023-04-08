#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <memory>
#include <cstring>
#include <unistd.h>
#include <string.h>
#include "Log.hpp"

#define BACK_LOG 64

class Sock
{
public:
    Sock()
    {
    }
    static int Socket()
    {
        // 初始化将端口号设立并进行相关绑定
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        int val = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEPORT | SO_REUSEADDR, &val, sizeof(val));
        if (sock < 0)
        {
            logMessage(FATAL, "sock create failed");
            exit(2);
        }
        return sock;
    }
    static void Bind(int sock, uint16_t port, std::string ip = "0.0.0.0")
    {
        // 进行sock结构体数据段的填充
        sockaddr_in serverSockaddr;
        memset(&serverSockaddr, 0, sizeof(serverSockaddr));
        serverSockaddr.sin_family = AF_INET;
        serverSockaddr.sin_addr.s_addr = inet_addr(ip.c_str());
        serverSockaddr.sin_port = htons(port);
        // 绑定sock
        if (bind(sock, (sockaddr *)&serverSockaddr, (socklen_t)sizeof(serverSockaddr)) < 0)
        {
            logMessage(FATAL, "sock bind failed");
            exit(0);
        }
    }
    static void Listen(int sock)
    {
        // 绑定成功
        logMessage(NORMAL, "sock create sucess");
        // 设置sock为listen状态
        if (listen(sock, BACK_LOG) < 0)
        {
            logMessage(FATAL, "listen error, %d:%s", errno, strerror(errno));
            exit(4);
        }
        // 监听成功, 等待启动
    }
    static int Accept(int sock, std::string *ip, uint16_t *port)
    {
        sockaddr_in client;
        memset(&client, 0, sizeof(client));
        socklen_t len = sizeof(client);
        int serverSock = accept(sock, (sockaddr *)&client, &len);
        if (serverSock < 0)
        {
            logMessage(ERROR, "accept error, %d:%s", errno, strerror(errno));
            exit(1);
        }
        logMessage(DEBUG, "%s connect sucess, on port: %d, socket: %d",
                   inet_ntoa(client.sin_addr), ntohs(client.sin_port), serverSock);
        if (port)
            *port = ntohs(client.sin_port);
        if (ip)
            *ip = inet_ntoa(client.sin_addr);
        return serverSock;
    }
    static bool Connect(int sock, std::string ip, uint16_t port)
    {
        sockaddr_in server;
        memset(&server, 0, sizeof(server));
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = inet_addr(ip.c_str());
        server.sin_port = htons(port);
        if (connect(sock, (sockaddr *)&server, sizeof(server)) < 0)
        {
            logMessage(ERROR, "connect error, %d:%s", errno, strerror(errno));
            exit(2);
        }
        return true;
    }
    ~Sock() {}
};
