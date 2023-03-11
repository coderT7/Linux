#pragma once
#include "Sock.hpp"
#include <vector>
#include <functional>
#include <pthread.h>

namespace m_netcal
{
    using func_t = std::function<void(int)>;
    class TcpServer;

    class ThreadData
    {
    public:
        ThreadData() {}
        ThreadData(int sock, TcpServer *server)
            : _sock(sock), _server(server)
        {
        }

    public:
        int _sock;
        TcpServer *_server;
    };

    class TcpServer
    {
    private:
        static void *routine(void *args)
        {
            pthread_detach(pthread_self());
            ThreadData *td = (ThreadData *)args;
            td->_server->execute(td->_sock);
            close(td->_sock);
            return nullptr;
        }

    public:
        TcpServer(const uint16_t &port, const std::string &ip = "0.0.0.0")
        {
            // 服务器进行初始化
            _listenSock = _sock.Socket();
            _sock.Bind(_listenSock, ip, port);
            _sock.Listen(_listenSock);
        }
        ~TcpServer()
        {
            if (_listenSock >= 0)
            {
                close(_listenSock);
            }
        }
        // 向服务器添加方法
        void setServer(func_t func)
        {
            func_s.push_back(func);
        }
        // 执行服务器要执行的方法
        void execute(int sock)//BUG1
        {
            for (auto &val : func_s)
            {
                // val(_listenSock);
                val(sock);
            }
        }
        // 服务器启动
        void start()
        {
            logMessage(DEBUG, "服务器启动");
            while (true)
            {
                std::string ip;
                uint16_t port;
                int serverSock = _sock.Accept(_listenSock, &ip, &port);
                if (serverSock < 0)
                {
                    logMessage(WARNING, "accept error");
                    continue;
                }
                ThreadData *td = new ThreadData(serverSock, this);
                pthread_t serverId;
                pthread_create(&serverId, nullptr, routine, td);
            }
        }

    private:
        Sock _sock;                 // 封装的socket接口, 便于socket的相关操作
        int _listenSock;            // 服务器进行监听
        std::vector<func_t> func_s; // 服务器要执行的例程
    };
}