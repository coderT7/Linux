#include "ThreadPool/Log.hpp"
#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

void Usage(char* args[]){
    std::cout << args[0] << " ip port" << std::endl;
}

int main(int argc, char *args[])
{
    if (argc != 3)
    {
        Usage(args);
        exit(0);
    }
    while (true)
    {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0)
        {
            logMessage(FATAL, "client cread sock failed");
            exit(2);
        }
        sockaddr_in clientSockaddr;
        memset(&clientSockaddr, 0, sizeof(clientSockaddr));
        clientSockaddr.sin_family = AF_INET;
        clientSockaddr.sin_addr.s_addr = inet_addr(args[1]);
        clientSockaddr.sin_port = htons(atoi(args[2]));
        char recvBuf[1024] = {0};
        // 第一次向服务器端发起连接时, sock会自动进行端口的相关绑定
        if (connect(sock, (sockaddr *)&clientSockaddr, sizeof(clientSockaddr)) < 0)
        {
            logMessage(ERROR, "client connect failed");
        }

        std::cout << "请输入消息: ";
        std::string buffer;
        getline(std::cin, buffer);

        if(send(sock, buffer.c_str(), buffer.size(), 0) < 0){
            logMessage(WARNING, "client send failed");
        }
        sockaddr_in temp;
        memset(&temp, 0, sizeof(temp));
        socklen_t len = sizeof(temp);
        ssize_t s = recv(sock, recvBuf, sizeof(recvBuf), 0);
        if(s > 0){
            recvBuf[s] = 0;
            logMessage(NORMAL, recvBuf);
        }
        else if(s == 0){
            logMessage(NORMAL, "对端server关闭, 当前client退出");
        }else{
            logMessage(WARNING, "client recv failed");
        }
        close(sock);
        logMessage(DEBUG, "client sock close");
    }   
    return 0;
}