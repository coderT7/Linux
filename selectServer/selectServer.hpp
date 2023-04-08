#pragma once
#include <sys/select.h>
#include <time.h>
#include <iostream>
#include "Sock.hpp"
#include "Log.hpp"
#include <string>
using namespace std;

// #define SIZE sizeof(fd_set) * 8
#define SIZE 1024
#define FD_NONE -1
class selectServer
{
public:
    selectServer(uint16_t port = 8080)
        : _port(port)
    {
        _listensock = Sock::Socket();
        Sock::Bind(_listensock, _port);
        Sock::Listen(_listensock);
        // logMessage(DEBUG, "开始添加listensock: %d至select", _listensock);
        // // 将listensock添加到select中进行监听
        // FD_ZERO(_readfds);             // 清空set
        // FD_SET(_listensock, _readfds); // 添加listensock至set
        // struct timeval timeout = {1, 0};
        // select(_listensock + 1, _readfds, nullptr, nullptr, &timeout);
        // logMessage(DEBUG, "已将listensock:%d设置入select中", _listensock);
        // 并将listensock添加至数组中
        for (int i = 0; i < SIZE; i++)
        {
            _fds[i] = FD_NONE;
        }
        _fds[0] = _listensock;
    }
    void handlerEvents(const fd_set &rfds)
    {
        logMessage(DEBUG, "开始处理事件");
        for (int i = 0; i < SIZE; i++)
        {
            // 1. 去掉不合法的fd
            if (_fds[i] == FD_NONE)
                continue;
            // 2. 合法的就一定就绪了?不一定
            if (FD_ISSET(_fds[i], &rfds))
            {
                // 指定的fd，读事件就绪
                //  读事件就绪：连接时间到来，accept
                if (_fds[i] == _listensock)
                    acceptor();
                else
                    recever(i);
            }
        }
    }
    void acceptor()
    {
        string clientip;
        uint16_t clientport = 0;
        // listensock上面的读事件就绪了，表示可以读取了
        // 获取新连接了
        int sock = Sock::Accept(_listensock, &clientip, &clientport); // 这里在进行accept会不会阻塞？不会！
        if (sock < 0)
        {
            logMessage(WARNING, "accept error");
            return;
        }
        logMessage(DEBUG, "get a new line success : [%s:%d] : %d", clientip.c_str(), clientport, sock);
        // 将连接添加至数组中
        int pos = 1;
        for (pos = 1; pos < SIZE; pos++)
        {
            if (_fds[pos] == FD_NONE)
                break;
        }
        // 如果遍历到最后了都没位置则直接将该链接丢弃
        if (pos == SIZE)
        {
            logMessage(WARNING, "当前载荷已满");
            close(sock);
            return;
        }
        _fds[pos] = sock;
        logMessage(DEBUG, "有连接到来, 成功添加sock:%d至fds", sock);
    }
    void recever(int pos)
    {
        char buf[1024];
        ssize_t n = recv(_fds[pos], buf, sizeof(buf), 0);
        if (n > 0)
        {
            buf[n] = 0;
            logMessage(DEBUG, "%d sock receve# %s", _fds[pos], buf);
        }
        else if (n == 0)
        {
            logMessage(DEBUG, "对端关闭连接, 本端也关闭连接%d", _fds[pos]);
            close(_fds[pos]);
            _fds[pos] = FD_NONE;
        }
        else
        {
            logMessage(ERROR, "recever error: %d : %s", errno, strerror(errno));
            close(_fds[pos]);
        }
    }
    void debugPrint()
    {
        std::cout << "当前合法的文件描述符: ";
        for (int i = 0; i < SIZE; i++)
        {
            if (_fds[i] == FD_NONE)
                continue;
            else
                std::cout << _fds[i] << " ";
        }
        std::cout << std::endl;
    }
    void start()
    {
        while (true)
        {
            fd_set readfds;
            FD_ZERO(&readfds);
            // struct timeval timeout = {1, 0};
            // 循环等待事件, 并且每次都要更新描述词组set
            int maxFd = _listensock;
            for (int i = 0; i < SIZE; i++)
            {
                // 如果当前文件描述符非法则跳过
                if (_fds[i] == FD_NONE)
                    continue;
                // 合法则设置进描述集合中
                FD_SET(_fds[i], &readfds);
                // 获取到最大的文件描述符值
                if (_fds[i] > maxFd)
                    maxFd = _fds[i];
            }
            // 第一个参数为fd的最大值+1
            int n = select(maxFd + 1, &readfds, nullptr, nullptr, nullptr);
            switch (n)
            {
            case 0:
                logMessage(DEBUG, "time out...");
                break;
            case -1:
                logMessage(WARNING, "select error: %d : %s", errno, strerror(errno));
                break;
            default:
                debugPrint();
                // 遍历fd数组, 查找哪一个fd有事件就绪了
                handlerEvents(readfds);

                break;
            }
        }
    }
    ~selectServer()
    {
        if (_listensock > 0)
        {
            close(_listensock);
        }
    }

private:
    int _listensock;
    // 服务器端口号
    uint16_t _port;
    // 默认绑定ip为0.0.0.0
    // 保存所有合法文件描述符的数组(用于更新select的参数)
    int _fds[SIZE];
};