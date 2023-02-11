#pragma once

#include "Thread.hpp"
#include "Log.hpp"
#include "LockGuard.hpp"
#include <queue>
#include <string>
#include <vector>

static const size_t g_thread_num = 3;

template <class T>
class ThreadPool
{
public:
    // 一般只有一个线程池, 故此设计为单例模式
    static ThreadPool<T> *getIntance(int num = g_thread_num)
    {
        // 双重判断, 减少反复的加锁和解锁操作(因为只有第一次调用该方法时才会有线程安全问题, 之后都不会)
        // 加多一个判断, 后续再来的线程时, 则不需要再进行加解锁, 只用做一次判断即可(拦截了大量的无用的访问锁的行为->只有第一次访问锁有意义)
        if (pool == nullptr)
        {
            LockGuard lock(&_smtx);
            if (pool == nullptr)
            {
                pool = new ThreadPool<T>(num);
            }
        }

        return pool;
    }
    bool isEmpty()
    {
        return _taskQueue.empty();
    }
    T getTask()
    {
        T task = _taskQueue.front();
        _taskQueue.pop();
        return task;
    }
    pthread_mutex_t *getMutex()
    {
        return &_mtx;
    }
    void waitCond()
    {
        pthread_cond_wait(&_cond, &_mtx);
    }
    void run()
    {
        for (auto &it : _threads)
        {
            it->start();
            logMessage(NORMAL, "%s %s", it->name(), "启动成功");
        }
    }
    // 线程池要推送任务至任务队列中
    void pushTask(const T &t)
    {
        LockGuard lock(&_mtx);
        _taskQueue.push(t);
        pthread_cond_signal(&_cond);
    }

    void deletePool()
    {
        pool->~ThreadPool();
        delete pool;
        pool = nullptr;
    }

private:
    ThreadPool(const ThreadPool<T> &pool) = delete;
    const ThreadPool<T> &operator=(const ThreadPool<T> &pool) = delete;
    ThreadPool(size_t num = g_thread_num)
        : threadNum(num)
    {
        pthread_mutex_init(&_mtx, nullptr);
        pthread_cond_init(&_cond, nullptr);
        for (int i = 1; i <= num; i++)
        {
            _threads.push_back(new Thread(i, routinue, this));
        }
    }
    ~ThreadPool()
    {
        pthread_mutex_destroy(&_mtx);
        pthread_cond_destroy(&_cond);
        for (auto &it : _threads)
        {
            it->join();
            delete it;
        }
        printf("test");
    }
    // 线程要执行的例程
    static void *routinue(void *args)
    {
        // 通过传入例程的参数提取到线程数据
        ThreadData *data = static_cast<ThreadData *>(args);
        // 通过线程数据获取到线程池
        ThreadPool<T> *pool = static_cast<ThreadPool<T> *>(data->args);
        while (true)
        {
            T task;
            {
                LockGuard lock(pool->getMutex());
                while (pool->isEmpty())
                    pool->waitCond();
                task = pool->getTask();
            }
            task(data->name);
        }
        return nullptr;
    }

private:
    static ThreadPool<T> *pool;
    static pthread_mutex_t _smtx;
    std::vector<Thread *> _threads;
    std::queue<T> _taskQueue;
    size_t threadNum;
    pthread_mutex_t _mtx;
    pthread_cond_t _cond;
};
template <typename T>
ThreadPool<T> *ThreadPool<T>::pool = nullptr;
template <typename T>
pthread_mutex_t ThreadPool<T>::_smtx = PTHREAD_MUTEX_INITIALIZER;