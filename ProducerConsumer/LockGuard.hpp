#pragma once

#include <pthread.h>
#include <iostream>

class Metux
{
public:
    Metux(pthread_mutex_t* pmx)
        : _pmx(pmx) {}

    void lock()
    {
        // std::cout << "加锁" << std::endl;
        pthread_mutex_lock(_pmx);
    }
    void unlock()
    {
        // std::cout << "解锁" << std::endl;
        pthread_mutex_unlock(_pmx);
    }
    ~Metux(){}
private:
    pthread_mutex_t *_pmx;
};
class LockGuard
{
public:
    LockGuard(pthread_mutex_t* pmx):
        _metux(pmx)
    {
        _metux.lock();
    }
    ~LockGuard(){
        _metux.unlock();
    }
private:
    Metux _metux;
};