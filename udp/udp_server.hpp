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
#include <unordered_map>
#include "Log.hpp"

class UdpServer
{
public:
    UdpServer(uint16_t port, std::string ip = "")
        : _port(port), _ip(ip)
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
        socketAddr.sin_addr.s_addr = _ip.empty() ? INADDR_ANY : inet_addr(_ip.c_str()); // 使用默认的IP进行绑定, 则可接收任意IP发送给对应端口的消息
        socketAddr.sin_port = htons(_port);                                             // port也是要转成对应网络字节序
        socketAddr.sin_family = AF_INET;                                                // 网络协议簇
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
        char key[64];
        memset(buffer, 0, sizeof(buffer));
        // 服务器启动后即是一个永不退出的后台进程
        while (true)
        {
            struct sockaddr_in clientSocket;
            memset(&clientSocket, 0, sizeof(clientSocket));
            socklen_t lenth;
            // 首先需要能读取数据
            ssize_t n = recvfrom(_socket, (void *)buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&clientSocket, &lenth);
            buffer[n] = '\0';
            if (n > 0)
            {
                // 将client端的ip和port拼接成key值
                uint16_t cli_port = ntohs(clientSocket.sin_port);              // 从网络中来的！
                std::string cli_ip = inet_ntoa(clientSocket.sin_addr);         // 4字节的网络序列的IP->本主机的字符串风格的IP，方便显示
                snprintf(key, sizeof(key), "%s-%u", cli_ip.c_str(), cli_port); // 127.0.0.1-8080
                logMessage(NORMAL, "key: %s", key);
                auto iter = _users.find(key);
                if (iter == _users.end())
                {
                    // exists
                    logMessage(NORMAL, "add new user : %s", key);
                    _users[key] = clientSocket;
                }
                // 同时将收到的信息广播给所有用户
                for (auto &iter : _users)
                {
                    std::string sendMessage = key;
                    sendMessage += "# 这是服务器发回的消息: ";
                    sendMessage += buffer;
                    logMessage(NORMAL, "push message to %s", iter.first.c_str());
                    sendto(_socket, sendMessage.c_str(), sendMessage.size(), 0, (struct sockaddr *)&(iter.second), sizeof(iter.second));
                }
            }
        }
    }
    ~UdpServer()
    {
        if (_socket > 0)
            close(_socket);
    }

private:
    int _socket;
    std::string _ip;
    uint16_t _port;
    // 进行群聊的用户广播
    std::unordered_map<std::string, sockaddr_in> _users;
};