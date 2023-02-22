#pragma once

#include <pthread.h>
#include <string>
#include "Log.hpp"

typedef void *(*funt)(void *);

class ThreadData
{
public:
    void *args;
    std::string name;
};

class Thread
{
public:
    Thread(int num, funt callback, void *args)
        : _num(num), _fun(callback)
    {
        char buffer[128] = "\0";
        snprintf(buffer, sizeof(buffer), "Thread - %d", num);
        _name = buffer;
        _data.args = args;
        _data.name = _name;
    }
    void start()
    {
        pthread_create(&_pid, nullptr, _fun, (void *)&_data);
        // logMessage(DEBUG, "%s - 创建成功", _data.name.c_str());
    }
    void join()
    {
        pthread_join(_pid, nullptr);
    }
    const char *name()
    {
        return _name.c_str();
    }
    ~Thread()
    {
    }

private:
    pthread_t _pid;
    funt _fun;
    int _num;
    std::string _name;
    ThreadData _data;
};