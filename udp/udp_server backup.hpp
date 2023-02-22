#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include "Log.hpp"

class UdpServer
{
public:
    UdpServer(int port)
        : _port(port)
    {
    }
    bool initServer()
    {
        // 初始化服务器
        // 创建socket
        _socket = socket(AF_INET, SOCK_DGRAM, 0);
        // socket创建失败则直接退出
        if (socket < 0)
        {
            logMessage(FATAL, "create socket failed.");
            exit(1);
        }
        // 创建成功后填充对应socket的结构体字段
        struct sockaddr_in socketAddr; // 用于网络通信的socket结构体
        memset(&socketAddr, 0, sizeof(socketAddr));
        // 初始化后对应进行字段的填充
        // socketAddr.sin_addr.s_addr = inet_addr(_ip.c_str()); // ip要从点分十进制的字符串对应转为四字节整数, 且要转换成网络字节序
        socketAddr.sin_addr.s_addr = INADDR_ANY; // 使用默认的IP进行绑定, 则可接收任意IP发送给对应端口的消息
        socketAddr.sin_port = htons(_port);      // port也是要转成对应网络字节序
        socketAddr.sin_family = AF_INET;         // 网络协议簇
        logMessage(NORMAL, "socket creadted");
        // socket创建完毕后对应绑定即可
        int ret = bind(_socket, (struct sockaddr *)&socketAddr, sizeof(socketAddr));
        if (ret < 0)
        {
            logMessage(FATAL, "socket bind failed");
            exit(2);
        }
        logMessage(DEBUG, "server inited");
        return true;
    }
    void startServer()
    {
        char buffer[256];
        memset(buffer, 0, sizeof(buffer));
        // 服务器启动后即是一个永不退出的后台进程
        while (true)
        {
            struct sockaddr_in clientSocket;
            memset(&clientSocket, 0, sizeof(clientSocket));
            socklen_t lenth;
            // 首先需要能读取数据
            ssize_t n = recvfrom(_socket, (void *)buffer, sizeof(buffer), 0, (struct sockaddr *)&clientSocket, &lenth);
            buffer[n] = '\0';
            if (n > 0)
            {
                logMessage(NORMAL, "%s-%d send message: %s", inet_ntoa(clientSocket.sin_addr), ntohs(clientSocket.sin_port), buffer);
                // 读取数据后调用接口执行指令
                FILE *pf = popen(buffer, "r");
                std::string sendBack;
                // 将执行结果返回给client
                while (fgets(buffer, sizeof(buffer), pf))
                {
                    sendBack += buffer;
                }
                ssize_t ret = sendto(_socket, sendBack.c_str(), sendBack.size(), 0, (struct sockaddr *)&clientSocket, lenth);
                if (ret < 0)
                {
                    logMessage(ERROR, "send message back was wrong.");
                }
            }
        }
    }
    ~UdpServer()
    {
        close(_socket);
    }

private:
    int _socket;
    std::string _ip;
    uint16_t _port;
};