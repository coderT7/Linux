#pragma once
#include <iostream>
#include <sys/epoll.h>
#include <functional>
#include "Log.hpp"
#include "Sock.hpp"
#include "Epoll.hpp"

#define DEFAULT_PORT 8080
#define SIZE 128
#define g_num 128
#define TIME_OUT 1000

class epollServer
{
    using func_t = std::function<void(std::string)>;

public:
    epollServer(func_t approach, uint16_t port = DEFAULT_PORT)
        : _port(port), _maxevents(g_num), _approach(approach), _timeout(TIME_OUT)
    {
        // 要对接收就绪队列以及最大队列长度进行初始化, 否则会报错
        // 使用封装的epoll接口
        _events = new struct epoll_event[_maxevents];
        _listenSock = Sock::Socket();
        Sock::Bind(_listenSock, _port);
        Sock::Listen(_listenSock);
        // 创建epoll模型
        _epfd = Epoll::EpollCreate(SIZE);
        // 将listenSock添加至模型中
        int n = Epoll::EpollCtrl(_epfd, EPOLL_CTL_ADD, _listenSock, EPOLLIN);
        if (n == 0)
            logMessage(DEBUG, "creak a linksock:%d && add to epoll:%d success", _listenSock, _epfd);
        else
        {
            logMessage(DEBUG, "creak a linksock:%d && add to epoll:%d failed", _listenSock, _epfd);
        }
    }
    void start()
    {
        while (true)
        {
            sleep(1);
            int n = Epoll::EpollWait(_epfd, _events, _maxevents, _timeout);
            switch (n)
            {
            case 0:
                logMessage(DEBUG, "time out...");
                break;
            case -1:
                logMessage(DEBUG, "epoll wait error: %d : %s", errno, strerror(errno));
                break;
            default:
                handler();
                break;
            }
        }
    }
    void handler()
    {
        // 处理事件
        logMessage(DEBUG, "start handle things.");
        for (int i = 0; i < _maxevents; i++)
        {
            if (_events[i].events & EPOLLIN)
            {
                if (_events[i].data.fd == _listenSock)
                    acceptor();
                else
                    receiver(i);
            }else if(_events[i].events & EPOLLOUT){
                // Reactor实现中完成
            }
        }
    }
    void acceptor()
    {
        std::string clientIp;
        uint16_t clientPort;
        int sock = Sock::Accept(_listenSock, &clientIp, &clientPort);
        logMessage(DEBUG, "get a new link success");
        struct epoll_event ev;
        ev.data.fd = sock;
        ev.events = EPOLLIN;
        int n = epoll_ctl(_epfd, EPOLL_CTL_ADD, sock, &ev);
        if (n == 0)
            logMessage(DEBUG, "creak a linksock:%d && add to epoll:%d success", _listenSock, _epfd);
        else
        {
            logMessage(DEBUG, "creak a linksock:%d && add to epoll:%d failed", _listenSock, _epfd);
        }
    }
    void receiver(int pos)
    {
        int sock = _events[pos].data.fd;
        char buf[1024];
        ssize_t n = recv(sock, buf, sizeof(buf), 0);
        if (n > 0)
        {
            buf[n] = 0;
            logMessage(DEBUG, "%d receive# %s", sock, buf);
            _approach(buf);
        }
        else if (n == 0)
        {
            logMessage(DEBUG, "client close, me too.");
            close(_events[pos].data.fd);
            _events[pos].data.fd = -1;
            _events[pos].events = 0;
        }
        else
        {
            logMessage(WARNING, "recv fail. %d : %s", errno, strerror(errno));
            close(_events[pos].data.fd);
            _events[pos].data.fd = -1;
            _events[pos].events = 0;
        }
    }
    ~epollServer()
    {
        if (_listenSock > 0)
            close(_listenSock);
        if (_epfd > 0)
            close(_listenSock);
    }

private:
    // epoll模型的fd
    int _epfd;
    int _listenSock;
    // 端口号
    uint16_t _port;
    // 就绪的事件及个数
    struct epoll_event *_events;
    // 该服务器处理方法
    func_t _approach;
    int _maxevents;
    int _timeout;
};