#pragma once
#include <poll.h>
#include <time.h>
#include <iostream>
#include "Sock.hpp"
#include "Log.hpp"
#include <string>
using namespace std;

#define FD_NONE -1
class pollServer
{
    const static int nfds = 100;

public:
    pollServer(uint16_t port = 8080)
        : _port(port), _nfds(nfds)
    {
        _listensock = Sock::Socket();
        Sock::Bind(_listensock, _port);
        Sock::Listen(_listensock);
        // 添加listensock至poll中托管
        _fds = new struct pollfd[_nfds];
        for (int i = 0; i < _nfds; i++)
        {
            _fds[i].fd = FD_NONE;
            _fds[i].events = 0;
            _fds[i].revents = 0;
        }
        _fds[0].fd = _listensock;
        _fds[0].events = POLLIN;
    }
    void handlerEvents()
    {
        logMessage(DEBUG, "开始处理事件");
        for (int i = 0; i < _nfds; i++)
        {
            if (_fds[i].fd == FD_NONE)
                continue;
            // 若返回事件内关心了POLLIN则进行处理
            if(_fds[i].revents & POLLIN){
                if(_fds[i].fd == _listensock)
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
        for (pos = 1; pos < _nfds; pos++)
        {
            if (_fds[pos].fd == FD_NONE)
                break;
        }
        // 如果遍历到最后了都没位置则直接将该链接丢弃
        if (pos == _nfds)
        {
            logMessage(WARNING, "当前载荷已满");
            close(sock);
            return;
        }
        // 将其添加至poll中监听
        _fds[pos].fd = sock;
        _fds[pos].events = POLLIN;
        logMessage(DEBUG, "有连接到来, 成功添加sock:%d至fds", sock);
    }
    void recever(int pos)
    {
        logMessage(DEBUG, "message in, get IO event: %d", _fds[pos].fd);

        char buf[1024];
        ssize_t n = recv(_fds[pos].fd, buf, sizeof(buf) - 1, 0);
        if (n > 0)
        {
            buf[n] = 0;
            logMessage(DEBUG, "%d sock receve# %s", _fds[pos].fd, buf);
        }
        else if (n == 0)
        {
            logMessage(DEBUG, "对端关闭连接, 本端也关闭连接%d", _fds[pos].fd);
            close(_fds[pos].fd);
            _fds[pos].fd = FD_NONE;
            _fds[pos].events = 0;
        }
        else
        {
            logMessage(ERROR, "recever error: %d : %s", errno, strerror(errno));
            close(_fds[pos].fd);
            _fds[pos].fd = FD_NONE;
            _fds[pos].events = 0;
        }
    }
    void debugPrint()
    {
        std::cout << "当前合法的文件描述符: ";
        // for (int i = 0; i < SIZE; i++)
        // {
        //     if (_fds[i] == FD_NONE)
        //         continue;
        //     else
        //         std::cout << _fds[i] << " ";
        // }
        // std::cout << std::endl;
    }
    void start()
    {
        while (true)
        {
            int n = poll(_fds, _nfds, 1000);
            switch (n)
            {
            case 0:
                logMessage(DEBUG, "time out...");
                break;
            case -1:
                logMessage(WARNING, "poll error: %d : %s", errno, strerror(errno));
                break;
            default:
                // debugPrint();
                handlerEvents();

                break;
            }
        }
    }
    ~pollServer()
    {
        if (_listensock > 0)
        {
            close(_listensock);
        }
        if(_fds)
            delete[] _fds;
    }

private:
    int _listensock;
    // 服务器端口号
    uint16_t _port;
    // 默认绑定ip为0.0.0.0
    // 有n个fd则有n个struct pollfd结构
    struct pollfd *_fds;
    // fd的数量(本质就是一个整型)
    nfds_t _nfds;
};