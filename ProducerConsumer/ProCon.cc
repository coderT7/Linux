#include <iostream>
#include <pthread.h>
#include "BlockQueue.hpp"
#include "Task.hpp"
#include <ctime>
#include <unistd.h>

// 生产者
void *producer(void *args)
{
    // 用于相近类型的类型转换 -> static_cast
    BlockQueue<Task> *bq = static_cast<BlockQueue<Task> *>(args);
    while(true){
        // 生产任务
        int x = rand() % 100;
        usleep(rand() % 1000);
        int y = rand() % 50;
        Task task(x, y, [](int x, int y)
                  { return x + y; });
        // 发布任务
        // push接口由设计时保证线程安全
        bq->push(task);
        std::cout << x << " + " << y << " = ?" << std::endl;
        sleep(1);
    }
    return nullptr;
}
// 消费者
void *consumer(void *args)
{
    BlockQueue<Task> *bq = static_cast<BlockQueue<Task> *>(args);
    while(true){
        // 消费任务(获取任务)
        Task task;
        bq->pop(&task);
        // 处理任务
        std::cout << task.getX() << " + " << task.getY() << " = " << task() << std::endl;
    }
    return nullptr;
}

int main()
{
    //生成随机数种子, 创建任务
    srand((unsigned int)time(nullptr) ^ getpid() ^ 0x77327);
    // 创建两个线程
    pthread_t pro, con;
    // 创建阻塞队列, 让两个线程看到同一个阻塞队列
    BlockQueue<Task> bq;
    pthread_create(&pro, nullptr, producer, &bq);
    pthread_create(&pro, nullptr, consumer, &bq);

    // 等待回收线程
    pthread_join(pro, nullptr);
    pthread_join(con, nullptr);

    return 0;
}
