#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unordered_map>
#include <string>
#include <memory>
#include <cstring>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "ThreadPool/Log.hpp"
#include "ThreadPool/ThreadPool.hpp"
#include "ThreadPool/Task.hpp"
#include "ThreadPool/Thread.hpp"
#include <pthread.h>
#include <ctype.h>

#define BACK_LOG 128

static void server(std::string name, int serverSock, const sockaddr_in &client)
{
    char buffer[1024];
    // 对client提供服务
    while (true)
    {
        // 接收client消息
        ssize_t s = read(serverSock, buffer, sizeof(buffer));
        if (s > 0)
        {
            logMessage(NORMAL, "[%s] %s send message# %s", name.c_str(), inet_ntoa(client.sin_addr), buffer);
            // 读到的消息进行回显
            buffer[s] = 0;
            if (strcmp(buffer, "quit") == 0)
            {
                logMessage(NORMAL, "get quit message");
                break;
            }
            std::string message;
            message += name;
            message += " - server回显# ";
            message += buffer;
            if (write(serverSock, message.c_str(), message.size()) < 0)
            {
                logMessage(WARNING, "server write failed");
            }
        }
        else if (s == 0)
        {
            // 对端关闭
            logMessage(NORMAL, "对端client关闭, 当前子线程 %s server关闭", name.c_str());
            break;
        }
        else
        {
            logMessage(ERROR, "read socket error, %d:%s", errno, strerror(errno));
            break;
        }
    }
    close(serverSock);
    return;
}

static void change(std::string name, int serverSock, const sockaddr_in &client)
{
    char buffer[1024];
    // 对client提供服务

    // 接收client消息
    ssize_t s = read(serverSock, buffer, sizeof(buffer));
    if (s > 0)
    {
        logMessage(NORMAL, "[%s] %s send message# %s", name.c_str(), inet_ntoa(client.sin_addr), buffer);
        // 读到的消息进行回显
        buffer[s] = 0;
        for (int i = 0; i < s; i++)
        {
            if(isalpha(buffer[i])){
                buffer[i] = toupper(buffer[i]);
            }
        }
        std::string message;
        message += name;
        message += " - server回显# ";
        message += buffer;
        if (write(serverSock, message.c_str(), message.size()) < 0)
        {
            logMessage(WARNING, "server write failed");
        }
    }
    else if (s == 0)
    {
        // 对端关闭
        logMessage(NORMAL, "对端client关闭, 当前子线程 %s server关闭", name.c_str());
    }
    else
    {
        logMessage(ERROR, "read socket error, %d:%s", errno, strerror(errno));
    }
    close(serverSock);
    logMessage(DEBUG, "server sock close");
    return;
}
static void dictionary(std::string name, int serverSock, const sockaddr_in &client)
{
    static std::unordered_map<std::string, std::string> dict;

    dict.insert({"apple", "苹果"});
    dict.insert({"banana", "香蕉"});
    dict["hello"] = "你好";
    char buffer[1024];
    // 对client提供服务

    // 接收client消息
    ssize_t s = read(serverSock, buffer, sizeof(buffer));
    if (s > 0)
    {
        logMessage(NORMAL, "[%s] %s send message# %s", name.c_str(), inet_ntoa(client.sin_addr), buffer);
        // 读到的消息进行回显
        buffer[s] = 0;
        auto ret = dict.find(buffer);

        std::string message;
        message += name;
        message += " - server回显# ";
        if (ret == dict.end())
        {
            message += "该单词尚未存储在词典中,请手动添加: ";
            message += buffer;
        }else{
            message += dict[buffer];
        }
        if (write(serverSock, message.c_str(), message.size()) < 0)
        {
            logMessage(WARNING, "server write failed");
        }
    }
    else if (s == 0)
    {
        // 对端关闭
        logMessage(NORMAL, "对端client关闭, 当前子线程 %s server关闭", name.c_str());
    }
    else
    {
        logMessage(ERROR, "read socket error, %d:%s", errno, strerror(errno));
    }
    close(serverSock);
    logMessage(DEBUG, "server sock close");
    return;
}
// class ThreadData
// {
// public:
//     ThreadData(const sockaddr_in &sockAddr, int sock)
//         : _sockAddr(sockAddr), _sock(sock)
//     {
//     }

// public:
//     sockaddr_in _sockAddr;
//     int _sock;
// };
// static void *rotinue(void *args)
// {
//     // 线程分离, 主线程不需要等待
//     pthread_detach(pthread_self());
//     ThreadData *td = (ThreadData *)args;
//     int serverSock = td->_sock;
//     sockaddr_in sockAddr = td->_sockAddr;
//     server(serverSock, sockAddr);
//     close(serverSock);
//     return nullptr;
// }

class Tcp_Server
{
public:
    // 设置忽略子进程退出信号, 直接交给操作系统回收
    Tcp_Server(uint16_t port, std::string ip = "0.0.0.0")
        : _port(port), 
        _ip(ip), 
        _threadPoolPtr(ThreadPool<Task>::getIntance())
    {
 
    }
    ~Tcp_Server() {}

    void server_init()
    {
        // 初始化将端口号设立并进行相关绑定
        _sock = socket(AF_INET, SOCK_STREAM, 0);
        if (_sock < 0)
        {
            logMessage(FATAL, "sock create failed");
            exit(2);
        }
        // 进行sock结构体数据段的填充
        sockaddr_in serverSockaddr;
        memset(&serverSockaddr, 0, sizeof(serverSockaddr));
        serverSockaddr.sin_family = AF_INET;
        serverSockaddr.sin_addr.s_addr = inet_addr(_ip.c_str());
        serverSockaddr.sin_port = htons(_port);
        // 绑定sock
        if (bind(_sock, (sockaddr *)&serverSockaddr, (socklen_t)sizeof(serverSockaddr)) < 0)
        {
            logMessage(FATAL, "sock bind failed");
            exit(0);
        }
        // 绑定成功
        logMessage(NORMAL, "sock create sucess");
        // 设置sock为listen状态
        if (listen(_sock, BACK_LOG) < 0)
        {
            logMessage(FATAL, "listen error, %d:%s", errno, strerror(errno));
            exit(4);
        }
        // 监听成功, 等待启动
    }
    // 多进程版本
    // void server_start()
    // {
    //     // 主动忽略子进程退出则子进程退出时会自动被回收
    //     // signal(SIGCHLD, SIG_IGN);
    //     // 将server启动
    //     while (true)
    //     {
    //         sockaddr_in client;
    //         memset(&client, 0, sizeof(client));
    //         socklen_t len = sizeof(client);
    //         int serverSock = accept(_sock, (sockaddr *)&client, &len);
    //         if(serverSock < 0)
    //         {
    //             logMessage(ERROR, "accept error, %d:%s", errno, strerror(errno));
    //             continue;
    //         }
    //         logMessage(DEBUG, "%s connect sucess, on port: %d", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
    //         pid_t pid = fork();
    //         if (pid == 0)
    //         {
    //             // 也可利用孤儿进程会被1号进程领养的方式实现多进程
    //             close(_sock);
    //             if(fork() > 0)
    //                 exit(0);
    //             server(serverSock, client);
    //             exit(0);
    //         }
    //         close(serverSock);
    //     }
    // }

    // 多线程版本
    // void server_start()
    // {
    //     // 主动忽略子进程退出则子进程退出时会自动被回收
    //     // signal(SIGCHLD, SIG_IGN);
    //     // 将server启动
    //     while (true)
    //     {
    //         sockaddr_in client;
    //         memset(&client, 0, sizeof(client));
    //         socklen_t len = sizeof(client);
    //         int serverSock = accept(_sock, (sockaddr *)&client, &len);
    //         if (serverSock < 0)
    //         {
    //             logMessage(ERROR, "accept error, %d:%s", errno, strerror(errno));
    //             continue;
    //         }
    //         logMessage(DEBUG, "%s connect sucess, on port: %d", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
    //         pthread_t threadId;
    //         ThreadData td(client, serverSock);
    //         logMessage(DEBUG, "线程创建成功,线程ID: %ld", threadId);
    //         pthread_create(&threadId, nullptr, rotinue, (void *)&td);
    //     }
    // }

    // 进程池版本
    void server_start()
    {
        _threadPoolPtr->run();
        while (true)
        {
            sockaddr_in client;
            memset(&client, 0, sizeof(client));
            socklen_t len = sizeof(client);
            int serverSock = accept(_sock, (sockaddr *)&client, &len);
            if (serverSock < 0)
            {
                logMessage(ERROR, "accept error, %d:%s", errno, strerror(errno));
                continue;
            }
            logMessage(DEBUG, "%s connect sucess, on port: %d, socket: %d", 
            inet_ntoa(client.sin_addr), ntohs(client.sin_port), serverSock);
            Task t(serverSock, client, dictionary);
            _threadPoolPtr->pushTask(t);
        }
    }

private:
    std::unique_ptr<ThreadPool<Task>> _threadPoolPtr;
    int _sock;
    std::string _ip;
    uint16_t _port;
};