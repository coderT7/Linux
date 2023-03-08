#pragma once

#include <iostream>
#include <functional>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
// typedef std::function<int(int, int)> func;
using func = std::function<void(std::string, int, sockaddr_in)>;

class Task
{
public:
    Task(){};

    void operator()(const std::string& name)
    {
        _func(name, _sock, _sockAddr);
    }

    Task(int sock, sockaddr_in sockAddr, func fun)
        : _func(fun), _sock(sock), _sockAddr(sockAddr)
    {
    }


public:
    int _sock;
    sockaddr_in _sockAddr;
    func _func;
};
