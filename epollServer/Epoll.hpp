#pragma once
#include <sys/epoll.h>
#include <iostream>
class Epoll
{
public:
    // 对epoll各接口的封装
    static int EpollCreate(int size)
    {
        int efd = epoll_create(size);
        if(efd >= 0)
            return efd;
        exit(5);
    }
    static bool EpollCtrl(int epfd, int op, int fd, int event)
    {
        struct epoll_event ev;
        ev.data.fd = fd;
        ev.events = event;
        int n = epoll_ctl(epfd, op, fd, &ev);
        return n == 0;
    }
    static int EpollWait(int epfd, struct epoll_event* events, int maxevents, int timeout)
    {
        return epoll_wait(epfd, events, maxevents, timeout);
    }
};
