#include <queue>
#include <string>
#include <pthread.h>
#include"LockGuard.hpp"
#define DEF_CAP 7

// 阻塞队列
template <class T>
class BlockQueue
{
private:
    bool isFull()
    {
        return _bq.size() == _capacity;
    }
    bool isEmpty()
    {
        return _bq.size() == 0;
    }

public:
    BlockQueue(int capacity = DEF_CAP) : _capacity(capacity)
    {
        // 初始化锁和条件变量
        pthread_mutex_init(&_pmx, nullptr);
        pthread_cond_init(&_full, nullptr);
        pthread_cond_init(&_empty, nullptr);
    }
    void push(const T &data)
    {
        // 生产
        // 加锁, 访问临界资源(检测临界资源)
        // pthread_mutex_lock(&_pmx);
        LockGuard lockGuard(&_pmx);
        // 防止伪唤醒或者异常唤醒状况需要使用while循环检测是否真的满足条件被唤醒
        while (isFull())
        {
            // 如果阻塞队列满了则在此等待
            pthread_cond_wait(&_full, &_pmx);
        }
        // 未满或者被唤醒则生产
        _bq.push(data);
        // 生产了则唤醒消费者
        pthread_cond_signal(&_empty);
        // 解锁
        // pthread_mutex_unlock(&_pmx);
    }
    void pop(T *data)
    {
        // 消费, data为输出型参数
        // 加锁消费
        // pthread_mutex_lock(&_pmx);
        LockGuard lockGuard(&_pmx);
        while (isEmpty())
        {
            // 如果队列为空则需要在此等待
            pthread_cond_wait(&_empty, &_pmx);
        }
        // 取出数据
        *data = _bq.front();
        _bq.pop();
        // 唤醒生产者
        pthread_cond_signal(&_full);
        // pthread_mutex_unlock(&_pmx);
    }

    ~BlockQueue()
    {
        // 将锁和条件变量销毁
        pthread_mutex_destroy(&_pmx);
        pthread_cond_destroy(&_full);
        pthread_cond_destroy(&_empty);
    }

private:
    // 队列
    std::queue<T> _bq;
    // 阻塞队列容量
    int _capacity;
    // 互斥锁
    pthread_mutex_t _pmx;
    // 条件变量
    pthread_cond_t _full;  // 队列满了
    pthread_cond_t _empty; // 队列为空
};